#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

enum class Gender {
	FEMALE,
	MALE
};

struct Person {
	int age;  // возраст
	Gender gender;  // пол
	bool is_employed;  // имеет ли работу
};

template <typename InputIt>
int ComputeMedianAge(InputIt range_begin, InputIt range_end) {
	if (range_begin == range_end) {
		return 0;
	}
	vector<typename InputIt::value_type> range_copy(range_begin, range_end);
	auto middle = begin(range_copy) + range_copy.size() / 2;
	nth_element(
			begin(range_copy), middle, end(range_copy),
			[](const Person& lhs, const Person& rhs) {
				return lhs.age < rhs.age;
			}
	);
	return middle->age;
}

void PrintStats(vector<Person> persons) {
	cout << "Median age = "
		<< ComputeMedianAge(begin(persons), end(persons)) << endl;

	auto fem_begin = partition(begin(persons), end(persons),
		[](const Person& p) { return p.gender == Gender::MALE; });
	cout << "Median age for females = "
		<< ComputeMedianAge(fem_begin, end(persons)) << endl;
	cout << "Median age for males = "
		<< ComputeMedianAge(begin(persons), fem_begin) << endl;

	auto fem_unemployed_begin = partition(fem_begin, end(persons),
		[](const Person& p) { return p.is_employed; });
	cout << "Median age for employed females = "
		<< ComputeMedianAge(fem_begin, fem_unemployed_begin) << endl;
	cout << "Median age for unemployed females = "
		<< ComputeMedianAge(fem_unemployed_begin, end(persons)) << endl;

	auto males_unemployed_begin = partition(begin(persons), fem_begin,
		[](const Person& p) { return p.is_employed; });
	cout << "Median age for employed males = "
		<< ComputeMedianAge(begin(persons), males_unemployed_begin) << endl;
	cout << "Median age for unemployed males = "
		<< ComputeMedianAge(males_unemployed_begin, fem_begin) << endl;

}

int main() {
	vector<Person> persons = {
		{31, Gender::MALE, false},
		{40, Gender::FEMALE, true},
		{24, Gender::MALE, true},
		{20, Gender::FEMALE, true},
		{80, Gender::FEMALE, false},
		{78, Gender::MALE, false},
		{10, Gender::FEMALE, false},
		{55, Gender::MALE, true},
	};
	PrintStats(persons);
	return 0;
}

