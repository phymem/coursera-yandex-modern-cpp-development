#include <string>
#include <vector>
#include <set>

#include <iostream>
#include <sstream>

using namespace std;

class Learner {
private:

	set<string> dict;

public:

	int Learn(const vector<string>& words) {
		int ret = 0;
		for (const auto& w : words) {
			if (!dict.count(w)) {
				++ret;
				dict.insert(w);
			}
		}
		return ret;
	}

	vector<string> KnownWords() {
		return { dict.begin(), dict.end() };
	}
};

int main() {
	Learner learner;
	string line;
	while (getline(cin, line)) {
		vector<string> words;
		stringstream ss(line);
		string word;
		while (ss >> word) {
			words.push_back(word);
		}
		cout << learner.Learn(words) << "\n";
	}
	cout << "=== known words ===\n";
	for (auto word : learner.KnownWords()) {
		cout << word << "\n";
	}
}
