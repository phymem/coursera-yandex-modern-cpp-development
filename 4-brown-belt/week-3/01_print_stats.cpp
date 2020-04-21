#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <unordered_map>

using namespace std;

template <typename Iterator>
class IteratorRange {
public:
	IteratorRange(Iterator begin, Iterator end)
		: first(begin)
		, last(end)
	{
	}

	Iterator begin() const {
		return first;
	}

	Iterator end() const {
		return last;
	}

private:
	Iterator first, last;
};

template <typename Collection>
auto Head(Collection& v, size_t top) {
	return IteratorRange{v.begin(), next(v.begin(), min(top, v.size()))};
}

struct Person {
	string name;
	int age, income;
	bool is_male;
};

vector<Person> ReadPeople(istream& input) {
	int count;
	input >> count;

	vector<Person> result(count);
	for (Person& p : result) {
		char gender;
		input >> p.name >> p.age >> p.income >> gender;
		p.is_male = gender == 'M';
	}

	return result;
}

pair<string, string> GetPopularName(const vector<Person>& people) {
	struct Data {
		unordered_map<string, int> names_count;
		unsigned int max_count = 0;
		string max_name;
	};

	Data stat[2];

	for (const auto& p : people) {
		Data* d = &stat[p.is_male];

		int count = ++d->names_count[p.name];
		if (count == d->max_count) {
			if (p.name < d->max_name)
				d->max_name = p.name;
		}
		else if (count > d->max_count) {
			d->max_count = count;
			d->max_name = p.name;
		}
	}

	return make_pair(stat[0].max_name, stat[1].max_name);
}

int main() {
	const vector<Person> people = ReadPeople(cin);

	const vector<Person> people_by_age = [&people]() {
		vector<Person> ret(people);
		sort(ret.begin(), ret.end(),
			[](const Person& lhs, const Person& rhs) {
				return lhs.age < rhs.age;
			}
		);
		return ret;
	}();

	const vector<Person> people_by_income = [&people]() {
		vector<Person> ret(people);
		sort(ret.begin(), ret.end(),
			[](const Person& lhs, const Person& rhs) {
				return lhs.income > rhs.income;
			}
		);
		return ret;
	}();

	const auto [popular_female_name, popular_male_name] = GetPopularName(people);

	for (string command; cin >> command; ) {
		if (command == "AGE") {
			int adult_age;
			cin >> adult_age;

			auto it = lower_bound(people_by_age.begin(), people_by_age.end(), adult_age,
				[](const Person& person, int age) { return person.age < age; });

			cout << "There are " << distance(it, people_by_age.end())
				<< " adult people for maturity age " << adult_age << '\n';

		} else if (command == "WEALTHY") {
			int count;
			cin >> count;

			auto head = Head(people_by_income, count);

			cout << "Top-" << count << " people have total income "
				<< accumulate(head.begin(), head.end(), 0,
					[](int total, const Person& p)
					{ return total += p.income; }) << "\n";

		} else if (command == "POPULAR_NAME") {
			char gender;
			cin >> gender;

			const string& popular_name = (gender == 'M')
				? popular_male_name : popular_female_name;

			if (popular_name.empty()) {
				cout << "No people of gender " << gender << "\n";
			}
			else {
				cout << "Most popular name among people of gender "
					<< gender << " is " << popular_name << '\n';
			}
		}
	}
}
