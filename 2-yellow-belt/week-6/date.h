#pragma once

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

class Date {
public:

	Date(int yy, int mm, int dd) :
		m_year(yy), m_month(mm), m_day(dd) {}

	int GetYear() const { return m_year; }
	int GetMonth() const { return m_month; }
	int GetDay() const { return m_day; }

	friend bool operator < (const Date& lhs, const Date& rhs);
	friend bool operator == (const Date& lhs, const Date& rhs);

private:

	int m_year;
	int m_month;
	int m_day;
};

inline bool operator < (const Date& lhs, const Date& rhs) {
	if (lhs.m_year != rhs.m_year)
		return lhs.m_year < rhs.m_year;
	if (lhs.m_month != rhs.m_month)
		return lhs.m_month < rhs.m_month;
	return lhs.m_day < rhs.m_day;
}

inline bool operator == (const Date& lhs, const Date& rhs) {
	return lhs.m_year == rhs.m_year
		&& lhs.m_month == rhs.m_month
		&& lhs.m_day == rhs.m_day;
}

inline ostream& operator << (ostream& os, const Date& date) {
	return os << setw(4) << setfill('0') << date.GetYear() << '-'
		<< setw(2) << setfill('0') << date.GetMonth() << '-'
		<< setw(2) << setfill('0') << date.GetDay();
}

Date ParseDate(istream& is);
