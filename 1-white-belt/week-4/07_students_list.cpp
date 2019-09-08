#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Student {
	string m_name;
	string m_birthday;

	Student(const string& name, const string& birthday) :
		m_name(name), m_birthday(birthday) {}
};

int main() {
	int n;
	cin >> n;

	vector<Student> students;
	students.reserve(n);

	while (n--) {
		string n1, n2, dd, mm, yy;
		cin >> n1 >> n2 >> dd >> mm >> yy;
		students.push_back(
			Student(n1 + " " + n2, dd + "." + mm + "." + yy));
	}

	cin >> n;
	while (n--) {
		string cmd;
		unsigned ndx;
		cin >> cmd >> ndx;
		ndx--;

		if (cmd == "name" && ndx < students.size())
			cout << students[ndx].m_name << "\n";
		else if (cmd == "date" && ndx < students.size())
			cout << students[ndx].m_birthday << "\n";
		else
			cout << "bad request" << "\n";
	}

	return 0;
}
