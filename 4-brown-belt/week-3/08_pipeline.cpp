#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
	string from;
	string to;
	string body;
};

ostream& operator << (ostream& out, const Email& email) {
	return out << email.from << "\n" << email.to << "\n" << email.body << "\n";
}

class Worker {
public:
	virtual ~Worker() = default;
	virtual void Process(unique_ptr<Email> email) = 0;
	virtual void Run() {
		// только первому worker-у в пайплайне нужно это имплементировать
		throw logic_error("Unimplemented");
	}

protected:
	// реализации должны вызывать PassOn, чтобы передать объект дальше
	// по цепочке обработчиков
	void PassOn(unique_ptr<Email> email) { // XXX const
		if (next_worker) {
			next_worker->Process(move(email));
		}
	}

public:

	void SetNext(unique_ptr<Worker> next) {
		next_worker = move(next);
	}

private:

	unique_ptr<Worker> next_worker;
};


class Reader : public Worker {
public:

	explicit Reader(istream& in) :
		input(in) {}

	void Process(unique_ptr<Email> email) override {
		throw logic_error("Reader::Process shouldn't be called");
	}

	void Run() override {
		for (Email email;
			getline(input, email.from)
			&& getline(input, email.to)
			&& getline(input, email.body); ) {
			PassOn(make_unique<Email>(email));
		}
	}

private:

	istream& input;
};


class Filter : public Worker {
public:
	using Function = function<bool(const Email&)>;

	explicit Filter(Function func) :
		predicate(func) {}

	void Process(unique_ptr<Email> email) override {
		if (predicate(*email)) {
			PassOn(move(email));
		}
	}

private:

	Function predicate;
};


class Copier : public Worker {
public:

	explicit Copier(string recipient_) :
		recipient(recipient_) {}

	void Process(unique_ptr<Email> email) override {
		Email copy(*email);
		PassOn(move(email));
		if (copy.to != recipient) {
			copy.to = recipient;
			PassOn(make_unique<Email>(copy));
		}
	}

private:

	string recipient;
};


class Sender : public Worker {
public:

	explicit Sender(ostream& out) : output(out) {}

	void Process(unique_ptr<Email> email) override {
		output << *email;
		PassOn(move(email));
	}

private:

	ostream& output;
};


// реализуйте класс
class PipelineBuilder {
public:

	// добавляет в качестве первого обработчика Reader
	explicit PipelineBuilder(istream& in) {
		workers.push_back(make_unique<Reader>(in));
	}

	// добавляет новый обработчик Filter
	PipelineBuilder& FilterBy(Filter::Function filter) {
		workers.push_back(make_unique<Filter>(filter));
		return *this;
	}

	// добавляет новый обработчик Copier
	PipelineBuilder& CopyTo(string recipient) {
		workers.push_back(make_unique<Copier>(recipient));
		return *this;
	}

	// добавляет новый обработчик Sender
	PipelineBuilder& Send(ostream& out) {
		workers.push_back(make_unique<Sender>(out));
		return *this;
	}

	// возвращает готовую цепочку обработчиков
	unique_ptr<Worker> Build() {
		for (size_t i = workers.size() - 1; i; --i) {
			workers[i - 1]->SetNext(move(workers[i]));
		}
		unique_ptr<Worker>ret = move(workers[0]);
		workers.clear();
		return ret;
	}

private:

	vector<unique_ptr<Worker>> workers;
};


void TestSanity() {
	string input = (
		"erich@example.com\n"
		"richard@example.com\n"
		"Hello there\n"

		"erich@example.com\n"
		"ralph@example.com\n"
		"Are you sure you pressed the right button?\n"

		"ralph@example.com\n"
		"erich@example.com\n"
		"I do not make mistakes of that kind\n"
	);
	istringstream inStream(input);
	ostringstream outStream;

	PipelineBuilder builder(inStream);
	builder.FilterBy([](const Email& email) {
		return email.from == "erich@example.com";
	});
	builder.CopyTo("richard@example.com");
	builder.Send(outStream);
	auto pipeline = builder.Build();

	pipeline->Run();

	string expectedOutput = (
		"erich@example.com\n"
		"richard@example.com\n"
		"Hello there\n"

		"erich@example.com\n"
		"ralph@example.com\n"
		"Are you sure you pressed the right button?\n"

		"erich@example.com\n"
		"richard@example.com\n"
		"Are you sure you pressed the right button?\n"
	);

	ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSanity);
	return 0;
}
