#include "test_runner.h"

#include <cassert>
#include <stdexcept>
#include <sstream>
#include <string>


void PrintJsonString(std::ostream& out, std::string_view str) {
	out << '"';
	for (char ch : str) {
		if (ch == '"' || ch == '\\') {
			out << '\\';
		}
		out << ch;
	}
	out << '"';
}

struct JsonValuePrinter {
	void operator() (std::ostream& out, int64_t n) {
		out << n;
	}
	void operator() (std::ostream& out, std::string_view sv) {
		PrintJsonString(out, sv);
	}
	void operator() (std::ostream& out, bool b) {
		out << (b  ? "true" : "false");
	}
	void operator() (std::ostream& out, std::nullptr_t) {
		out << "null";
	}
};

template <typename Parent>
class ObjectContext;

template <typename Parent>
class ArrayContext {
public:

	ArrayContext(std::ostream& out, Parent& parent) :
		out_(out), parent_(parent), delim_(""), closing_("]") { out_ << "["; }

	~ArrayContext() { EndArray(); }

	using Self = ArrayContext<Parent>;

	Self& Number(int64_t n) { return PrintValue(n); }
	Self& String(std::string_view s) { return PrintValue(s); }
	Self& Boolean(bool b) { return PrintValue(b); }
	Self& Null() { return PrintValue(nullptr); }

	Parent& EndArray() {
		out_ << closing_;
		closing_ = "";
		return parent_;
	}

	ArrayContext<Self> BeginArray() {
		PrintDelim();
		return ArrayContext<Self>(out_, *this);
	}

	ObjectContext<Self> BeginObject() {
		PrintDelim();
		return ObjectContext<Self>(out_, *this);
	}

private:

	void PrintDelim() {
		out_ << delim_;
		delim_ = ",";
	}

	template  <typename Value>
	Self& PrintValue(Value val) {
		PrintDelim();
		JsonValuePrinter{}(out_, val);
		return *this;
	}

	std::ostream& out_;
	Parent& parent_;

	const char* delim_;
	const char* closing_;
};

template <typename Parent>
class ObjectValue {
public:

	ObjectValue(std::ostream& out, Parent& parent) :
		out_(out), parent_(parent), value_("null") {}

	~ObjectValue() { out_ << value_; }

	Parent& Number(int64_t n) { return PrintValue(n); }
	Parent& String(std::string_view s) { return PrintValue(s); }
	Parent& Boolean(bool b) { return PrintValue(b); }
	Parent& Null() { return PrintValue(nullptr); }

	ArrayContext<Parent> BeginArray() {
		value_ = "";
		return ArrayContext<Parent>(out_, parent_);
	}

	ObjectContext<Parent> BeginObject() {
		value_ = "";
		return ObjectContext<Parent>(out_, parent_);
	}

private:

	template <typename Value>
	Parent& PrintValue(Value val) {
		JsonValuePrinter{}(out_, val);
		value_ = "";
		return parent_;
	}

	std::ostream& out_;
	Parent& parent_;

	const char* value_;
};

template <typename Parent>
class ObjectContext {
public:

	ObjectContext(std::ostream& out, Parent& parent) :
		out_(out), parent_(parent), delim_(""), closing_("}") { out << "{"; }

	~ObjectContext() { EndObject(); }

	using Self = ObjectContext<Parent>;

	ObjectValue<Self> Key(std::string_view sv) {
		out_ << delim_;
		delim_ = ",";
		PrintJsonString(out_, sv);
		out_ << ':';
		return ObjectValue<Self>(out_, *this);
	}

	Parent& EndObject() {
		out_ << closing_;
		closing_ = "";
		return parent_;
	}

private:

	std::ostream& out_;
	Parent& parent_;

	const char* delim_;
	const char* closing_;
};

struct Dummy {};

ArrayContext<Dummy> PrintJsonArray(std::ostream& out) {
	static Dummy dummy;
	return ArrayContext(out, dummy);
}

ObjectContext<Dummy> PrintJsonObject(std::ostream& out) {
	static Dummy dummy;
	return ObjectContext(out, dummy);
}

void TestArray() {
	{
		std::ostringstream out;

		PrintJsonArray(out)
		  .Null()
		  .String("Hello")
		  .Number(123)
		  .Boolean(false);

		ASSERT_EQUAL(out.str(), R"([null,"Hello",123,false])");
	}

	{
		std::ostringstream out;

		PrintJsonArray(out)
			.String("Hello")
			.BeginArray()
				.String("World");

		ASSERT_EQUAL(out.str(), R"(["Hello",["World"]])");
	}

	{
		std::ostringstream out;

		PrintJsonArray(out)
			.Number(5)
			.Number(6)
			.BeginArray()
				.Number(7)
			.EndArray()
			.Number(8)
			.String("bingo!");

		ASSERT_EQUAL(out.str(), R"([5,6,[7],8,"bingo!"])");
	}

	{
		std::ostringstream out;

		PrintJsonArray(out)
			.BeginArray()
				.BeginObject();

		ASSERT_EQUAL(out.str(), R"([[{}]])");
	}
}

void TestObject() {
	{
		std::ostringstream out;

		PrintJsonObject(out)
			.Key("foo"); // ends with 'null'

		ASSERT_EQUAL(out.str(), R"({"foo":null})");
	}

	{
		std::ostringstream out;

		PrintJsonObject(out)
			.Key("id1").Number(1234)
			.Key("id2").Boolean(false)
			.Key("").Null()
			.Key("\"").String("\\");

		ASSERT_EQUAL(out.str(), R"({"id1":1234,"id2":false,"":null,"\"":"\\"})");
	}

	{
		std::ostringstream out;

		PrintJsonObject(out)
			.Key("foo")
			.BeginArray()
				.String("Hello")
			.EndArray()
			.Key("foo")
				.BeginObject()
					.Key("foo");

		ASSERT_EQUAL(out.str(), R"({"foo":["Hello"],"foo":{"foo":null}})");
	}
}

int main() {

/*
	// compilation errors

	PrintJsonArray(std::cout)
		.Key("foo")
		.BeginArray()
		.EndArray()
		.EndArray();

	PrintJsonArray(std::cout)
		.EndArray()
		.BeginArray();

	PrintJsonObject(std::cout)
		.String("foo");

	PrintJsonObject(std::cout)
		.Key("foo")
		.Key("bar");

	PrintJsonObject(std::cout)
		.EndArray();

	PrintJsonObject(std::cout)
		.EndObject()
		.BeginObject();
*/

	TestRunner tr;
	RUN_TEST(tr, TestArray);
	RUN_TEST(tr, TestObject);

	return 0;
}
