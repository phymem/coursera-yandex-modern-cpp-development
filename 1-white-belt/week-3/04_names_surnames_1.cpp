#define DBG

#ifdef DBG
#include <iostream>
#include <string>
using namespace std;
#endif

#include <map>

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

#ifdef DBG
int main() {

	Person person;
	person.ChangeFirstName(1965, "Polina");
	person.ChangeLastName(1967, "Sergeeva");
	for (int year : {1900, 1965, 1990}) {
		cout << person.GetFullName(year) << endl;
	}

	person.ChangeFirstName(1970, "Appolinaria");
	for (int year : {1969, 1970}) {
		cout << person.GetFullName(year) << endl;
	}

	person.ChangeLastName(1968, "Volkova");
	for (int year : {1969, 1970}) {
		cout << person.GetFullName(year) << endl;
	}

	return 0;
}
#endif
