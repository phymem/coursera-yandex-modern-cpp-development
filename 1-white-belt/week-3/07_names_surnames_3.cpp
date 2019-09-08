// #define DBG

#ifdef DBG
#include <iostream>
#include <string>
using namespace std;
#endif

#include <map>

class Person {
public:
	Person(const string& first_name, const string& last_name, int birth_year) :
		m_birth_year(birth_year)
	{
		m_first_name[m_birth_year] = first_name;
		m_last_name[m_birth_year] = last_name;
	}

	void ChangeFirstName(int year, const string& first_name) {
		if (year >= m_birth_year) m_first_name[year] = first_name;
	}
	void ChangeLastName(int year, const string& last_name) {
		if (year >= m_birth_year) m_last_name[year] = last_name;
	}
	string GetFullName(int year) const {
		if (year < m_birth_year)
			return "No person";

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
	string GetFullNameWithHistory(int year) const {
		if (year < m_birth_year)
			return "No person";

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

	string FindName(const map<int, string>& m, int year) const {
		string ret;
		for (const auto& p : m) {
			if (p.first <= year)
				ret = p.second;
			else
				break;
		}
		return ret;
	}

	string FindNameWithHistory(const map<int, string>& m, int year) const {
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

private:

	int m_birth_year;

	map<int, string> m_first_name;
	map<int, string> m_last_name;
};

#ifdef DBG
int main() {

	Person person("Polina", "Sergeeva", 1960);
	for (int year : {1959, 1960}) {
		cout << person.GetFullNameWithHistory(year) << endl;
	}

	person.ChangeFirstName(1965, "Appolinaria");
	person.ChangeLastName(1967, "Ivanova");
	for (int year : {1965, 1967}) {
		cout << person.GetFullNameWithHistory(year) << endl;
	}

	return 0;
}
#endif
