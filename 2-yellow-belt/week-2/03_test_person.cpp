#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

template <class T>
ostream& operator << (ostream& os, const vector<T>& s) {
	os << "{";
	bool first = true;
	for (const auto& x : s) {
		if (!first) {
			os << ", ";
		}
		first = false;
		os << x;
	}
	return os << "}";
}

template <class T>
ostream& operator << (ostream& os, const set<T>& s) {
	os << "{";
	bool first = true;
	for (const auto& x : s) {
		if (!first) {
			os << ", ";
		}
		first = false;
		os << x;
	}
	return os << "}";
}

template <class K, class V>
ostream& operator << (ostream& os, const map<K, V>& m) {
	os << "{";
	bool first = true;
	for (const auto& kv : m) {
		if (!first) {
			os << ", ";
		}
		first = false;
		os << kv.first << ": " << kv.second;
	}
	return os << "}";
}

template<class T, class U>
void AssertEqual(const T& t, const U& u, const string& hint = {}) {
	if (t != u) {
		ostringstream os;
		os << "Assertion failed: " << t << " != " << u;
		if (!hint.empty()) {
			os << " hint: " << hint;
		}
		throw runtime_error(os.str());
	}
}

void Assert(bool b, const string& hint) {
	AssertEqual(b, true, hint);
}

class TestRunner {
public:
	template <class TestFunc>
	void RunTest(TestFunc func, const string& test_name) {
		try {
			func();
			cerr << test_name << " OK" << endl;
		} catch (exception& e) {
			++fail_count;
			cerr << test_name << " fail: " << e.what() << endl;
		} catch (...) {
			++fail_count;
			cerr << "Unknown exception caught" << endl;
		}
	}

	~TestRunner() {
		if (fail_count > 0) {
			cerr << fail_count << " unit tests failed. Terminate" << endl;
			exit(1);
		}
	}

private:
	int fail_count = 0;
};

class Person {
public:
	void ChangeFirstName(int year, const string& first_name) {
		m_first_name[year] = first_name;
	}
	void ChangeLastName(int year, const string& last_name) {
		m_last_name[year] = last_name;
	}
	string GetFullName(int year) {
		string fname = FindName(m_first_name, year);
		string lname = FindName(m_last_name, year);

		if (fname.empty() && lname.empty())
			return "Incognito";
		if (fname.empty())
			return lname + " with unknown first name";
		if (lname.empty())
			return fname + " with unknown last name";
		return fname + " " + lname;
	}
private:

	string FindName(const map<int, string>& m, int year) {
		string ret;
		for (const auto& p : m) {
			if (p.first <= year)
				ret = p.second;
			else
				break;
		}
		return ret;
	}

	map<int, string> m_first_name;
	map<int, string> m_last_name;
};

void Test_Empty() {
	Person p;
	AssertEqual(p.GetFullName(0), "Incognito", "GetFullName(0)");
	AssertEqual(p.GetFullName(100), "Incognito", "GetFullName(0)");
}

void Test_Incognito() {
	{
		Person p;
		p.ChangeFirstName(10, "first");
		p.ChangeLastName(10, "last");
		AssertEqual(p.GetFullName(0), "Incognito", "(10, 10) GetFullName(0)");
		AssertEqual(p.GetFullName(5), "Incognito", "(10, 10) GetFullName(5)");
		AssertEqual(p.GetFullName(9), "Incognito", "(10, 10) GetFullName(9)");
	}
	{
		Person p;
		p.ChangeFirstName(10, "first");
		p.ChangeLastName(11, "last");
		AssertEqual(p.GetFullName(0), "Incognito", "(10, 10) GetFullName(0)");
		AssertEqual(p.GetFullName(5), "Incognito", "(10, 10) GetFullName(5)");
		AssertEqual(p.GetFullName(9), "Incognito", "(10, 10) GetFullName(9)");
	}
}

void Test_UnknownLastName() {
	Person p;
	p.ChangeFirstName(10, "xxx");
	p.ChangeLastName(15, "last");
	AssertEqual(p.GetFullName(11), "xxx with unknown last name", "(10, 15) GetFullName(11)");
	AssertEqual(p.GetFullName(12), "xxx with unknown last name", "(10, 15) GetFullName(12)");
	AssertEqual(p.GetFullName(14), "xxx with unknown last name", "(10, 15) GetFullName(14)");
}

void Test_UnknownFirstName() {
	Person p;
	p.ChangeFirstName(15, "first");
	p.ChangeLastName(10, "xxx");
	AssertEqual(p.GetFullName(11), "xxx with unknown first name", "(15, 10) GetFullName(11)");
	AssertEqual(p.GetFullName(12), "xxx with unknown first name", "(15, 10) GetFullName(12)");
	AssertEqual(p.GetFullName(14), "xxx with unknown first name", "(15, 10) GetFullName(14)");
}

void Test_Change() {
	Person p;
	p.ChangeFirstName(10, "f1");
	p.ChangeFirstName(20, "f2");
	p.ChangeLastName(10, "l1");
	p.ChangeLastName(15, "l2");

	AssertEqual(p.GetFullName(10), "f1 l1", "GetFullName(10)");
	AssertEqual(p.GetFullName(12), "f1 l1", "GetFullName(12)");
	AssertEqual(p.GetFullName(15), "f1 l2", "GetFullName(15)");
	AssertEqual(p.GetFullName(17), "f1 l2", "GetFullName(17)");
	AssertEqual(p.GetFullName(20), "f2 l2", "GetFullName(20)");
	AssertEqual(p.GetFullName(25), "f2 l2", "GetFullName(25)");
}

int main() {
	TestRunner runner;

	runner.RunTest(Test_Empty, "Test_Empty");
	runner.RunTest(Test_Incognito, "Test_Incognito");
	runner.RunTest(Test_UnknownLastName, "Test_UnknownLastName");
	runner.RunTest(Test_UnknownFirstName, "Test_UnknownFirstName");
	runner.RunTest(Test_Change, "Test_Change");
	return 0;
}
