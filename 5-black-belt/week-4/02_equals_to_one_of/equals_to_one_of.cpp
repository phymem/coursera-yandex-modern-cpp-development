#include "test_runner.h"

// EqualsToOneOf(x, "apple", "orange") означает (x == "apple" || x == "orange")

// template <typename T, typename... Args>
// inline bool EqualsToOneOf(const T& x, const Args& ... args) {
//     return ((x == args) || ...); // fold expression
// }

template <typename A>
inline  bool EqualsToOneOf(const A&) {
	return false;
}

template <typename A, typename B, typename ... Args>
inline bool EqualsToOneOf(const A& a, const B& b, const Args& ... args) {
	return a == b || EqualsToOneOf(a, args ...);
}

void Test() {
	auto x = "pear";
	ASSERT(EqualsToOneOf(x, "pear"));
	ASSERT(!EqualsToOneOf(x, "apple"));
	ASSERT(EqualsToOneOf(x, "apple", "pear"));
	ASSERT(!EqualsToOneOf(x, "apple", "banana"));
	ASSERT(EqualsToOneOf(x, "apple", "banana", "pear"));
	ASSERT(!EqualsToOneOf(x, "apple", "banana", "peach"));
	ASSERT(EqualsToOneOf(x, "apple", "banana", "pear", "orange"));
	ASSERT(!EqualsToOneOf(x, "apple", "banana", "peach", "orange"));

	ASSERT(!EqualsToOneOf(1));
	ASSERT(EqualsToOneOf(1, 2.0, 1.0, 2));
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, Test);
	return 0;
}
