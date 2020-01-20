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

int GetDistinctRealRootCount(double a, double b, double c) {
	if (a == 0 && b == 0)
		return 0;

	if (a == 0) {
		return 1;
	} else if (b == 0 && c == 0) {
		return 1;
	} else if (b == 0) {
		double d = -c / a;
		if (d > 0) {
			return 2;
		}
	} else if (c == 0) {
		return 2;
	} else {
		double d = b * b - 4 * a * c;

		if (d == 0.0) {
			return 1;
		} else if (d > 0) {
			return 2;
		}
	}

	return 0;
}

void Test_A0() {
	AssertEqual(GetDistinctRealRootCount(0, 1,   1),   1, "0 1   1");
	AssertEqual(GetDistinctRealRootCount(0, 100, 100), 1, "0 100 100");

	AssertEqual(GetDistinctRealRootCount(0, 1,   0),   1, "0 1   0");
	AssertEqual(GetDistinctRealRootCount(0, 100, 0),   1, "0 100 0");
}

void Test_B0() {
	AssertEqual(GetDistinctRealRootCount(1,   0,  1),   0, "1   0 1");
	AssertEqual(GetDistinctRealRootCount(100, 0,  100), 0, "100 0 100");

	AssertEqual(GetDistinctRealRootCount(-1,   0, 1),   2, "-1   0 1");
	AssertEqual(GetDistinctRealRootCount(-100, 0, 100), 2, "-100 0 100");

	AssertEqual(GetDistinctRealRootCount(1,   0,  -1),   2, "1   0  -1");
	AssertEqual(GetDistinctRealRootCount(100, 0,  -100), 2, "100 0  -100");

	AssertEqual(GetDistinctRealRootCount(-1,   0, -1),   0, "-1   0  -1");
	AssertEqual(GetDistinctRealRootCount(-100, 0, -100), 0, "-100 0  -100");
}

void Test_C0() {
	AssertEqual(GetDistinctRealRootCount(1,   1,   0), 2, "1   1   0");
	AssertEqual(GetDistinctRealRootCount(100, 100, 0), 2, "100 100 0");
}

void Test_A0_B0() {
	AssertEqual(GetDistinctRealRootCount(0, 0, 1),   0, "0 0 1");
	AssertEqual(GetDistinctRealRootCount(0, 0, 100), 0, "0 0 100");
}

void Test_B0_C0() {
	AssertEqual(GetDistinctRealRootCount(1,   0, 0), 1, "1   0 0");
	AssertEqual(GetDistinctRealRootCount(100, 0, 0), 1, "100 0 0");
}

void Test_D() {
	AssertEqual(GetDistinctRealRootCount(1,   1,   1),   0, "1   1   1");
	AssertEqual(GetDistinctRealRootCount(100, 1,   1),   0, "100 1   1");
	AssertEqual(GetDistinctRealRootCount(1,   100, 1),   2, "1   100 1");
	AssertEqual(GetDistinctRealRootCount(1,   1,   100), 0, "1   1   100");

	AssertEqual(GetDistinctRealRootCount(1, 2, 1), 1, "1 2 1");
	AssertEqual(GetDistinctRealRootCount(2, 4, 2), 1, "2 4 2");
}

int main() {
	TestRunner runner;

	runner.RunTest(Test_A0, "Test_A0");
	runner.RunTest(Test_B0, "Test_B0");
	runner.RunTest(Test_A0_B0, "Test_A0_B0");
	runner.RunTest(Test_B0_C0, "Test_B0_C0");
	runner.RunTest(Test_C0, "Test_C0");
	runner.RunTest(Test_D, "Test_D");

	return 0;
}
