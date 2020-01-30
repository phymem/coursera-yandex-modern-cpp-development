#include "database.h"

#include <algorithm>
#include <set>

void Database::Add(const Date& date, const string& event) {
	if (event.size()) {
		auto it_m = m_db.find(date);
		if (it_m != end(m_db)) {
			auto it_e = it_m->second.find(event);
			if (it_e != end(it_m->second))
				return;
		}
	}
	m_db[date][event] = ++m_next_event_id;
}

bool Database::DeleteEvent(const Date& date, const string& event) {
	auto it_m = m_db.find(date);
	if (it_m != end(m_db)) {
		auto it_e = it_m->second.find(event);
		if (it_e != end(it_m->second)) {
			it_m->second.erase(it_e);
			return true;
		}
	}
	return false;
}

int Database::DeleteDate(const Date& date) {
	if (m_db.count(date)) {
		int ret = m_db[date].size();
		m_db.erase(date);
		return ret;
	}
	return 0;
}

vector<string> Database::Find(const Date& date) const {
	auto it = m_db.find(date);
	if (it == end(m_db))
		return {};

	map<event_id_t, string> m;
	for (const auto& e : it->second) {
		m[e.second] = e.first;
	}

	vector<string> ret;
	ret.reserve(m.size());
	for (const auto& e : m) {
		ret.push_back(e.second);
	}
	return ret;
}

void Database::Print(ostream& os) const {
	for (const auto& p : m_db) {
		map<event_id_t, string> m;
		for (const auto& e : p.second) {
			m[e.second] = e.first;
		}
		for (const auto& e : m) {
			os << p.first << " " << e.second << endl;
		}
	}
}

pair<Date, string> Database::Last(const Date& date) const {
	auto it = m_db.upper_bound(date);
	if (it == begin(m_db))
		throw invalid_argument("invalid date");
	--it;

	event_id_t max_id = 0;
	string max_name;

	for (const auto& e : it->second) {
		if (e.second > max_id) {
			max_name = e.first;
			max_id = e.second;
		}
	}

	return make_pair(it->first, max_name);
}
