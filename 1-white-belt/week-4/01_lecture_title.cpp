#include <string>

using namespace std;

struct Specialization {
	string value;
	explicit Specialization(const string& s) : value(s) {}
};
struct Course {
	string value;
	explicit Course(const string& s) : value(s) {}
};
struct Week {
	string value;
	explicit Week(const string& s) : value(s) {}
};
struct LectureTitle {
	string specialization;
	string course;
	string week;
	LectureTitle(const Specialization& s, const Course& c, const Week& w) :
		specialization(s.value), course(c.value), week(w.value) {}
};

int main() {

	LectureTitle t1(Specialization("C++"), Course("White belt"), Week("4th"));
/*
	LectureTitle t2("C++", "White belt", "4th");

	LectureTitle t3(string("C++"), string("White belt"), string("4th"));

	LectureTitle t4 = {"C++", "White belt", "4th"};

	LectureTitle t5 = {{"C++"}, {"White belt"}, {"4th"}};

	LectureTitle t6(Course("White belt"), Specialization("C++"), Week("4th"));

	LectureTitle t7(Specialization("C++"), Week("4th"), Course("White belt"));
*/
	return 0;
}
