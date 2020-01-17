#include <iostream>
#include <vector>
#include <map>

using namespace std;

template <typename T>
T Sqr(const T& v);
template <typename T>
vector<T> Sqr(const vector<T>& vec);
template <typename First, typename Second>
pair<First, Second> Sqr(const pair<First, Second>& v);
template <typename Key, typename Value>
map<Key, Value> Sqr(const map<Key, Value>& m);

template <typename T>
T Sqr(const T& v) {
	return v * v;
}

template <typename T>
vector<T> Sqr(const vector<T>& vec) {
	vector<T> ret;
	for (const auto& v : vec) {
		ret.push_back(Sqr(v));
	}
	return ret;
}

template <typename First, typename Second>
pair<First, Second> Sqr(const pair<First, Second>& v) {
	return make_pair(Sqr(v.first), Sqr(v.second));
}

template <typename Key, typename Value>
map<Key, Value> Sqr(const map<Key, Value>& m) {
	map<Key, Value> ret;
	for (const auto& p : m) {
		ret[p.first] = Sqr(p.second);
	}
	return ret;
}

int main() {

	vector<int> v = {1, 2, 3};
	cout << "vector:";
	for (int x : Sqr(v)) {
		cout << ' ' << x;
	}
	cout << endl;

	map<int, pair<int, int>> map_of_pairs = {
		{4, {2, 2}},
		{7, {4, 3}}
	};
	cout << "map of pairs:" << endl;
	for (const auto& x : Sqr(map_of_pairs)) {
		cout << x.first << ' ' << x.second.first << ' ' << x.second.second << endl;
	}

	return 0;
}
