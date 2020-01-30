#pragma once

#include "date.h"

#include <vector>
#include <map>

using namespace std;

inline ostream& operator << (ostream& os, const pair<Date, string>& p) {
	return os << p.first<< ' ' << p.second;
}

void TestDatabase_Add();
void TestDatabase_Find();
void TestDatabase_Last();
void TestDatabase_Remove();

class Database {
public:

	Database() : m_next_event_id(0) {}

	void Add(const Date& date, const string& event);

	bool DeleteEvent(const Date& date, const string& event);

	int DeleteDate(const Date& date);

	vector<string> Find(const Date& date) const;

	void Print(ostream& os) const;

	template <typename Predicate>
	int RemoveIf(Predicate predicate) {
		unsigned int ret = 0;
		auto it_m = begin(m_db);
		while (it_m != end(m_db)) {
			auto it_e = begin(it_m->second);
			while (it_e != end(it_m->second)) {
				if (predicate(it_m->first, it_e->first)) {
					it_e = it_m->second.erase(it_e);
					ret++;
				}
				else {
					++it_e;
				}
			}
			if (it_m->second.empty())
				it_m = m_db.erase(it_m);
			else
				++it_m;
		}
		return ret;
	}

	template <typename Predicate>
	vector<pair<Date, string>> FindIf(Predicate predicate) const {
		vector<pair<Date, string>> ret;
		for (const auto& p : m_db) {
			map<event_id_t, string> m;
			for (const auto& e : p.second) {
				if (predicate(p.first, e.first)) {
					m[e.second] = e.first;
				}
			}
			ret.reserve(ret.size() + m.size());
			for (const auto& e : m) {
				ret.push_back(make_pair(p.first, e.second));
			}
		}
		return ret;
	}

	pair<Date, string> Last(const Date& date) const;

private:

	typedef unsigned long long event_id_t;
	event_id_t m_next_event_id;

	map<Date, map<string, event_id_t>> m_db;
};
