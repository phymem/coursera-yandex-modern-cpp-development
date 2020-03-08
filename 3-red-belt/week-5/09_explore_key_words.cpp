#include "test_runner.h"
#include "profile.h"

#include <map>
#include <future>
#include <string>

using namespace std;

struct Stats {
	map<string, int> word_frequences;

	void operator += (const Stats& other) {
		for (const auto& p : other.word_frequences) {
			word_frequences[p.first] += p.second;
		}
	}
};

Stats ExploreLine(const set<string>& key_words, const string& line) {
	Stats ret;

	auto char_pos = line.find_first_not_of(' ');

	while (char_pos < line.size()) {
		auto space_pos = line.find_first_of(' ', char_pos);
		if (space_pos > line.size())
			space_pos = line.size();

		string word = line.substr(char_pos, space_pos - char_pos);
		if (key_words.count(word)) {
			ret.word_frequences[move(word)]++;
		}

		char_pos = line.find_first_not_of(' ', space_pos);
	}

	return ret;
}

Stats ExploreKeyWordsSingleThread(
	const set<string>& key_words, istream& input
) {
	Stats result;
	for (string line; getline(input, line); ) {
		result += ExploreLine(key_words, line);
	}
	return result;
}

Stats ExploreKeyWords(const set<string>& key_words, istream& input) {

	// one line per cpu thread
	vector<string> thread_lines(thread::hardware_concurrency());
	unsigned int counter = 0;
	for (string line; getline(input, line); ) {
		thread_lines[counter++ % thread_lines.size()] += line + ' ';
	}

	vector<future<Stats>> partial_stats;
	for (const auto& line : thread_lines) {
		partial_stats.push_back(async(
			ExploreLine, ref(key_words), ref(line)));
	}

	Stats ret;
	for (auto& s : partial_stats) {
		ret += s.get();
	}

	return ret;
}

void TestBasic() {
	const set<string> key_words = {"yangle", "rocks", "sucks", "all"};

	stringstream ss;
	ss << "this new yangle service really rocks\n";
	ss << "It sucks when yangle isn't available\n";
	ss << "10 reasons why yangle is the best IT company\n";
	ss << "yangle rocks others suck\n";
	ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

	const auto stats = ExploreKeyWords(key_words, ss);
	const map<string, int> expected = {
		{"yangle", 6},
		{"rocks", 2},
		{"sucks", 1}
	};
	ASSERT_EQUAL(stats.word_frequences, expected);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestBasic);
}
