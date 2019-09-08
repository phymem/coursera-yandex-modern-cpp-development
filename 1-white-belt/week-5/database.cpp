#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <set>

using namespace std;

class Date {
public:

	Date(int yy, int mm, int dd) :
		m_year(yy), m_month(mm), m_day(dd) {}

	int GetYear() const { return m_year; }
	int GetMonth() const { return m_month; }
	int GetDay() const { return m_day; }

private:

	int m_year;
	int m_month;
	int m_day;
};

bool operator < (const Date& lhs, const Date& rhs) {
	if (lhs.GetYear() == rhs.GetYear()) {
		if (lhs.GetMonth() == rhs.GetMonth())
			return lhs.GetDay() < rhs.GetDay();
		return lhs.GetMonth() < rhs.GetMonth();
	}
	return lhs.GetYear() < rhs.GetYear();
}

class Database {
public:
	void AddEvent(const Date& date, const string& event) {
		if (!event.empty()) m_events[date].insert(event);
	}

	bool DeleteEvent(const Date& date, const string& event) {
		if (m_events.count(date) && m_events[date].count(event)) {
			m_events[date].erase(event);
			return true;
		}
		return false;
	}

	int DeleteDate(const Date& date) {
		if (m_events.count(date)) {
			int ret = m_events[date].size();
			m_events.erase(date);
			return ret;
		}
		return 0;
	}

	set<string> Find(const Date& date) const {
		if (m_events.count(date))
			return m_events.at(date);
		return {};
	}

	void Print() const { // +
		for (const auto& p : m_events) {
			for (const auto& ev : p.second) {
				cout << setw(4) << setfill('0') << p.first.GetYear() << '-'
					<< setw(2) << setfill('0') << p.first.GetMonth() << '-'
					<< setw(2) << setfill('0') << p.first.GetDay() << ' ' << ev << endl;
			}
		}
	}

private:

	map<Date, set<string>> m_events;
};

Date ParseDate(const string& str)
{
	stringstream ss(str);

	int yy, mm, dd;

	if (!(ss >> yy) || ss.peek() != '-' || !ss.ignore(1)
		|| !(ss >> mm) || ss.peek() != '-' || !ss.ignore(1)
		|| !(ss >> dd) || !ss.eof())
		throw logic_error(string("Wrong date format: ") + str);

	if (mm < 1 || mm > 12)
		throw logic_error(string("Month value is invalid: ") + to_string(mm));

	if (dd < 1 || dd > 31)
		throw logic_error(string("Day value is invalid: ") + to_string(dd));

	return Date(yy, mm, dd);
}

int main() {
	try {
		Database db;

		string line;
		while (getline(cin, line)) {

			stringstream ss(line);
			string cmd;
			ss >> cmd;

			if (cmd == "Add") {
				string date, event;
				ss >> date >> event;
				db.AddEvent(ParseDate(date), event);
			}
			else if (cmd == "Del") {
				string date, event;
				ss >> date >> event;
				const Date key = ParseDate(date);
				if (event.empty()) {
					cout << "Deleted " << db.DeleteDate(key) << " events" << endl; // +
				}
				else {
					if (db.DeleteEvent(key, event))
						cout << "Deleted successfully" << endl;
					else
						cout << "Event not found" << endl;
				}
			}
			else if (cmd == "Find") {
				string date;
				ss >> date;
				for (const string& s : db.Find(ParseDate(date))) {
					cout << s << endl; // +
				}
			}
			else if (cmd == "Print") {
				db.Print();
			}
			else if (!cmd.empty()){
				throw logic_error("Unknown command: " + cmd);
			}
		}
	}
	catch (const exception& e) {
		cout << e.what() << endl;
	}

	return 0;
}
