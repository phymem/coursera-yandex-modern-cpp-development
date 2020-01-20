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

class Rational {
public:

	Rational() :
		numerator(0),
		denominator(1) {}

	Rational(int _numerator, int _denominator) :
		numerator(_numerator), denominator(_denominator)
	{
		if (!numerator) {
			denominator = 1;
		}
		else {
			if (denominator < 0) {
				numerator = -numerator;
				denominator = -denominator;
			}

			int gcd = GetGCD(abs(numerator), denominator);
			numerator /= gcd;
			denominator /= gcd;
		}
	}

	int Numerator() const { return numerator; }
	int Denominator() const { return denominator; }

private:

	int GetGCD(int a, int b) const {
		if (a > b) {
			int t = a;
			a = b;
			b = t;
		}
		while (int r = b % a) {
			b = a;
			a = r;
		}
		return a;
	}

private:

	int numerator;
	int denominator;
};

void Test_DefaultCtor() {
	Rational r;
	AssertEqual(r.Numerator(),  0, "numerator test");
	AssertEqual(r.Denominator(),  1, "denominator test");
}

void Test_Sign() {
	{
		Rational r(-1, -3);
		AssertEqual(r.Numerator(), 1, "-1/-3 numerator");
		AssertEqual(r.Denominator(), 3, "-1/-3 denominator");
	}
	{
		Rational r(-5, -7);
		AssertEqual(r.Numerator(), 5, "-5/-7 numerator");
		AssertEqual(r.Denominator(), 7, "-5/-7 denominator");
	}
	{
		Rational r(-1, 3);
		AssertEqual(r.Numerator(), -1, "-1/3 numerator");
		AssertEqual(r.Denominator(), 3, "-1/3 denominator");
	}
	{
		Rational r(-5, 7);
		AssertEqual(r.Numerator(), -5, "-5/7 numerator");
		AssertEqual(r.Denominator(), 7, "-5/7 denominator");
	}
	{
		Rational r(1, -3);
		AssertEqual(r.Numerator(), -1, "1/-3 numerator");
		AssertEqual(r.Denominator(), 3, "1/-3 denominator");
	}
	{
		Rational r(5, -7);
		AssertEqual(r.Numerator(), -5, "5/-7 numerator");
		AssertEqual(r.Denominator(), 7, "5/-7 denominator");
	}
}

void Test_Casting() {
	{
		Rational r(1, 3);
		AssertEqual(r.Numerator(), 1, "1/3 numerator");
		AssertEqual(r.Denominator(), 3, "1/3 denominator");
	}
	{
		Rational r(2, 5);
		AssertEqual(r.Numerator(), 2, "2/5 numerator");
		AssertEqual(r.Denominator(), 5, "2/5 denominator");
	}
	{
		Rational r(5, 10);
		AssertEqual(r.Numerator(), 1, "5/10 numerator");
		AssertEqual(r.Denominator(), 2, "5/10 denominator");
	}
	{
		Rational r(9, 3);
		AssertEqual(r.Numerator(), 3, "9/3 numerator");
		AssertEqual(r.Denominator(), 1, "9/3 denominator");
	}
}

void Test_ZeroDenominator() {
	{
		Rational r(0, 3);
		AssertEqual(r.Numerator(), 0, "0/3 numerator");
		AssertEqual(r.Denominator(), 1, "0/3 denominator");
	}
	{
		Rational r(0, -3);
		AssertEqual(r.Numerator(), 0, "0/-3 numerator");
		AssertEqual(r.Denominator(), 1, "0/-3 denominator");
	}
}

int main() {
	TestRunner runner;
	runner.RunTest(Test_DefaultCtor, "Test_DefaultCtor");
	runner.RunTest(Test_Sign, "Test_Sign");
	runner.RunTest(Test_Casting, "Test_Casting");
	runner.RunTest(Test_ZeroDenominator, "Test_ZeroDenominator");
	return 0;
}
