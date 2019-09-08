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
	string GetFullNameWithHistory(int year) {
		string fname = FindNameWithHistory(m_first_name, year);
		string lname = FindNameWithHistory(m_last_name, year);

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

	string FindNameWithHistory(const map<int, string>& m, int year) {
		string history;
		string name;

		for (const auto& p : m) {
			if (p.first <= year) {
				if (name.length() && name != p.second) {
					if (history.empty())
						history = name;
					else
						history = name + ", " + history;
				}
				name = p.second;
			}
			if (p.first >= year)
				break;
		}

		if (history.length())
			name += " (" + history + ")";

		return name;
	}

	map<int, string> m_first_name;
	map<int, string> m_last_name;
};

#ifdef DBG
int main() {

/*
	Person person;
	person.ChangeFirstName(1900, "Eugene");
	person.ChangeLastName(1900, "Sokolov");
	person.ChangeLastName(1910, "Sokolov");
	person.ChangeFirstName(1920, "Evgeny");
	person.ChangeLastName(1930, "Sokolov");

	cout << person.GetFullNameWithHistory(1940) << endl;
*/

	Person person;

	person.ChangeFirstName(1965, "Polina");
	person.ChangeLastName(1967, "Sergeeva");
	for (int year : {1900, 1965, 1990}) {
	cout << person.GetFullNameWithHistory(year) << endl;
	}

	person.ChangeFirstName(1970, "Appolinaria");
	for (int year : {1969, 1970}) {
	cout << person.GetFullNameWithHistory(year) << endl;
	}

	person.ChangeLastName(1968, "Volkova");
	for (int year : {1969, 1970}) {
	cout << person.GetFullNameWithHistory(year) << endl;
	}

	person.ChangeFirstName(1990, "Polina");
	person.ChangeLastName(1990, "Volkova-Sergeeva");
	cout << person.GetFullNameWithHistory(1990) << endl;

	person.ChangeFirstName(1966, "Pauline");
	cout << person.GetFullNameWithHistory(1966) << endl;

	person.ChangeLastName(1960, "Sergeeva");
	for (int year : {1960, 1967}) {
	cout << person.GetFullNameWithHistory(year) << endl;
	}

	person.ChangeLastName(1961, "Ivanova");
	cout << person.GetFullNameWithHistory(1967) << endl;

	return 0;
}
#endif
