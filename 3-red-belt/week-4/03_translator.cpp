#include "test_runner.h"
#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

class Translator {
public:

	void Add(string_view source, string_view target) {
		auto it_source = m_words.insert(string(source));
		auto it_target = m_words.insert(string(target));
		m_forward[*it_source.first] = *it_target.first;
		m_backward[*it_target.first] = *it_source.first;
	}

	string_view TranslateForward(string_view source) const {
		return Translate(m_forward, source);
	}
	string_view TranslateBackward(string_view target) const {
		return Translate(m_backward, target);
	}

private:

	string_view Translate(const map<string_view, string_view>& m, const string_view& s) const {
		if (auto it = m.find(s); it != end(m))
			return it->second;
		return {};
	}

private:

	map<string_view, string_view> m_forward;
	map<string_view, string_view> m_backward;

	set<string> m_words;
};

void TestSimple() {
	Translator translator;
	translator.Add(string("okno"), string("window"));
	translator.Add(string("stol"), string("table"));

	ASSERT_EQUAL(translator.TranslateForward("okno"), "window");
	ASSERT_EQUAL(translator.TranslateBackward("table"), "stol");
	ASSERT_EQUAL(translator.TranslateBackward("stol"), "");
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSimple);
	return 0;
}
