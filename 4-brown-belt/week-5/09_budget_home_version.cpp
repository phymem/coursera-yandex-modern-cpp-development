#include <iostream>
#include <cassert>
#include <ctime>
#include <map>

using namespace std;

const int DAY_SEC = 24 * 60 * 60;

inline time_t to_timestamp(int yy, int mm, int dd) {
	std::tm t{ 0 };
	t.tm_year = yy - 1900;
	t.tm_mon  = mm - 1;
	t.tm_mday = dd;
	return mktime(&t);
}

inline time_t read_timestamp(istream& is) {
	int yy, mm, dd;
	char ch;
	is >> yy >> ch >> mm >> ch >> dd;
	return to_timestamp(yy, mm, dd);
}

inline int diffdays(time_t from, time_t to) {
	return (to - from) / DAY_SEC;
}

class BudgetManager {
public:

	BudgetManager() {
		m_timeline[ to_timestamp(2000, 1, 1) ] = 0;
		m_timeline[ to_timestamp(2100, 1, 1) ] = 0;
	}
	~BudgetManager() {}

	void earn(time_t from, time_t to, double value) {
		split_timeline(to);

		timeline_t::iterator it = split_timeline(from);
		while (it->first != to) {
			timeline_t::iterator next_it = next(it);
			auto val = split_value(value, make_pair(from, to), next_it->first);

			it->second += val.first;
			value = val.second;
			from = next_it->first;
			it = next_it;
		}
	}

	void tax(time_t from, time_t to) {
		split_timeline(to);

		for (timeline_t::iterator it = split_timeline(from); it->first != to; ++it)
			it->second *= 0.87;
	}

	double income(time_t from, time_t to) {
		double ret = 0;
		split_timeline(to);
		for (timeline_t::iterator it = split_timeline(from); it->first != to; ++it) {
			ret += it->second;
		}
		return ret;
	}

	void dump_timeline(ostream& os) {
		unsigned int counter = 1;
		for (const auto& p : m_timeline) {
			os << counter++ << " " << p.first << " " << p.second << "\n";
		}
	}

private:

	typedef map<time_t, double> timeline_t;

	timeline_t::iterator split_timeline(time_t at_time) {
		timeline_t::iterator it = m_timeline.lower_bound(at_time);
		assert(it != m_timeline.end());

		if (it->first == at_time)
			return it;

		assert(it->first > at_time);
		timeline_t::iterator prev_it = prev(it);

		auto value = split_value(prev_it->second,
			 make_pair(prev_it->first, it->first), at_time);
		prev_it->second = value.first;
		return m_timeline.insert( make_pair(at_time, value.second) ).first;
	}

	pair<double, double> split_value(double value, pair<time_t, time_t> range, time_t at_time) {
		assert(range.first < range.second);

		if (range.first == at_time) {
			return pair<double, double>(0, value);
		}
		if (range.second == at_time) {
			return pair<double, double>(value, 0);
		}

		assert(range.first < at_time);
		assert(at_time < range.second);

		double first = (value / diffdays(range.first, range.second)) * diffdays(range.first, at_time);
		return make_pair(first, value - first);
	}

private:

	timeline_t m_timeline;
};

int main() {
	int num;
	cin >> num;

	BudgetManager manager;
	cout.precision(25);

	while (num--) {
		string query;
		cin >> query;

		if (query == "Earn") {
			time_t from = read_timestamp(cin);
			time_t to = read_timestamp(cin);
			double value;
			cin >> value;

			manager.earn(from, to + DAY_SEC, value);
		}
		else if (query == "PayTax") {
			time_t from = read_timestamp(cin);
			time_t to = read_timestamp(cin);

			manager.tax(from, to + DAY_SEC);
		}
		else if (query == "ComputeIncome") {
			time_t from = read_timestamp(cin);
			time_t to = read_timestamp(cin);

			cout << manager.income(from, to + DAY_SEC) << "\n";
		}
	}

	return 0;
}
