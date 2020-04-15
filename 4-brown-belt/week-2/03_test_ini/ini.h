#pragma once

#include <istream>
#include <unordered_map>
using namespace std;

namespace Ini {

using Section = unordered_map<string, string>;

class Document {
public:
	Section& AddSection(string name) {
		return sections[name];
	}
	const Section& GetSection(const string& name) const {
		return sections.at(name);
	}
	size_t SectionCount() const {
		return sections.size();
	}

private:
	unordered_map<string, Section> sections;
};

inline Document Load(istream& input) {
	Document doc;

	Section* section = nullptr;
	for (string line; getline(input, line); ) {
		if (line.empty())
			continue;

		if (line[0] == '[') {
			section = &doc.AddSection(line.substr(
				1 /*skip '['*/, line.find_first_of(']') - 1));
		}
		else {
			auto pos = line.find_first_of('=');
			(*section)[line.substr(0, pos)] = line.substr(pos + 1);
		}
	}

	return doc;
}

} // namespace Ini
