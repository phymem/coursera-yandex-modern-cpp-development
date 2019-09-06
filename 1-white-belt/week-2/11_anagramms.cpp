#include <iostream>
#include <map>

using namespace std;

map<char, int> BuildCharCounters(const string& str) {
	map<char, int> ret;
	for (char ch : str) {
		ret[ch]++;
	}
	return ret;
}

int main() {

	int n;
	cin >> n;

	while (n--) {
		string w1, w2;
		cin >> w1 >> w2;

		if (BuildCharCounters(w1) == BuildCharCounters(w2))
			cout << "YES";
		else
			cout << "NO";

		cout << '\n';
	}

	return 0;
}
