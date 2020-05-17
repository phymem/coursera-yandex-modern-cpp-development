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
		m_timeline[ to_timestamp(2000, 1, 1) ] = Record();
		m_timeline[ to_timestamp(2100, 1, 1) ] = Record();
	}
	~BudgetManager() {}

	void earn(time_t from, time_t to, double value) {
		update(from, to, value, [](Record& rec, double val) { rec.m_income += val; });
	}

	void spend(time_t from, time_t to, double value) {
		update(from, to, value, [](Record& rec, double val) { rec.m_expense += val; });
	}

	void tax(time_t from, time_t to, double value) {
		split_timeline(to);

		for (timeline_t::iterator it = split_timeline(from); it->first != to; ++it)
			it->second.m_income *= value;
	}

	double income(time_t from, time_t to) {
		split_timeline(to);

		Record result;
		for (timeline_t::iterator it = split_timeline(from); it->first != to; ++it) {
			result += it->second;
		}

		return result.m_income - result.m_expense;
	}

	void dump_timeline(ostream& os) {
		unsigned int counter = 1;
		for (const auto& p : m_timeline) {
			os << counter++ << " " << p.first << " "
				<< p.second.m_income << " "
				<< p.second.m_expense << "\n";
		}
	}

private:

	struct Record {
		double m_income;
		double m_expense;

		Record() : m_income(0), m_expense(0) {}

		Record(double income, double expense) :
			m_income(income), m_expense(expense) {}

		Record& operator += (const Record& rec) {
			m_income += rec.m_income;
			m_expense += rec.m_expense;
			return *this;
		}
	};

	typedef map<time_t, Record> timeline_t;

	timeline_t::iterator split_timeline(time_t at_time) {
		timeline_t::iterator it = m_timeline.lower_bound(at_time);
		assert(it != m_timeline.end());

		if (it->first == at_time)
			return it;

		assert(it->first > at_time);
		timeline_t::iterator prev_it = prev(it);

		auto income = split_value(prev_it->second.m_income,
			 make_pair(prev_it->first, it->first), at_time);

		auto expense = split_value(prev_it->second.m_expense,
			 make_pair(prev_it->first, it->first), at_time);

		prev_it->second = Record(income.first, expense.first);

		return m_timeline.insert( make_pair(at_time, Record(income.second, expense.second)) ).first;
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

	template <typename Updater>
	void update(time_t from, time_t to, double value, Updater updater) {
		split_timeline(to);

		timeline_t::iterator it = split_timeline(from);
		while (it->first != to) {
			timeline_t::iterator next_it = next(it);
			auto val = split_value(value, make_pair(from, to), next_it->first);

			updater(it->second, val.first);
			value = val.second;
			from = next_it->first;
			it = next_it;
		}
	}

private:

	timeline_t m_timeline;
};

enum QueryType {
	QT_NONE,
	QT_EARN,
	QT_SPEND,
	QT_PAY_TAX,
	QT_COMPUTE_INCOME
};

QueryType to_query_type(const string& query) {
	if (query == "Earn") return QT_EARN;
	if (query == "Spend") return QT_SPEND;
	if (query == "PayTax") return QT_PAY_TAX;
	if (query == "ComputeIncome") return QT_COMPUTE_INCOME;
	return QT_NONE;
}

int main() {
	int num;
	cin >> num;

	BudgetManager manager;
	cout.precision(25);

	while (num--) {
		string query;
		cin >> query;

		QueryType query_type = to_query_type(query);

		if (query_type == QT_COMPUTE_INCOME) {
			time_t from = read_timestamp(cin);
			time_t to = read_timestamp(cin);

			cout << manager.income(from, to + DAY_SEC) << "\n";
		}
		else {
			time_t from = read_timestamp(cin);
			time_t to = read_timestamp(cin);

			int value;
			cin >> value;

			switch (query_type) {
			case QT_EARN:
				manager.earn(from, to + DAY_SEC, value);
				break;
			case QT_SPEND:
				manager.spend(from, to + DAY_SEC, value);
				break;
			case QT_PAY_TAX:
				manager.tax(from, to + DAY_SEC, (100.0 - value) / 100.0);
				break;
			default: ;
			}
		}
	}

	return 0;
}
