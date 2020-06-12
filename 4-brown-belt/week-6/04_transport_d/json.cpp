#include "json.h"

#include <stdexcept>

using namespace std;

namespace Json {

Document::Document(Node root) : root(move(root)) {
}

const Node& Document::GetRoot() const {
	return root;
}

Node LoadNode(istream& input);

Node LoadArray(istream& input) {
	vector<Node> result;

	for (char c; input >> c && c != ']'; ) {
		if (c != ',') {
			input.putback(c);
		}
		result.push_back(LoadNode(input));
	}

	return Node(move(result));
}

Node LoadNumber(istream& input) {
	char buf[512];
	int ndx = 0;
	char sign = input.peek();
	if (sign == '+' || sign == '-') {
		buf[ndx++] = input.get();
	}
	while (isdigit(input.peek())) {
		buf[ndx++] = input.get();
		if (ndx == sizeof(buf))
			throw out_of_range("LoadNumber: matissa");
	}
	if (input.peek() == '.') {
		buf[ndx++] = input.get();
		if (ndx == sizeof(buf))
			throw out_of_range("LoadNumber: dot");

		while (isdigit(input.peek())) {
			buf[ndx++] = input.get();
			if (ndx == sizeof(buf))
				throw out_of_range("LoadNumber: exponent");
		}
	}
	buf[ndx] = '\0';
	return Node(string(buf));
}

Node LoadString(istream& input) {
	string line;
	getline(input, line, '"');
	return Node(move(line));
}

Node LoadDict(istream& input) {
	map<string, Node> result;

	for (char c; input >> c && c != '}'; ) {
		if (c == ',') {
			input >> c;
		}

		string key = LoadString(input).AsString();
		input >> c;
		result.emplace(move(key), LoadNode(input));
	}

	return Node(move(result));
}

Node LoadText(istream& input, const char* f_text) {
	for (const char* p = f_text; *p; ++p) {
		if (input.get() != *p)
			throw invalid_argument("LoadText: f_text");
	}
	return Node(string(f_text));
}

Node LoadNode(istream& input) {
	char c;
	input >> c;

	if (c == '[') {
		return LoadArray(input);
	} else if (c == '{') {
		return LoadDict(input);
	} else if (c == '"') {
		return LoadString(input);
	} else if (c == 'f') {
		input.putback(c);
		return LoadText(input, "false");
	} else if (c == 't') {
		input.putback(c);
		return LoadText(input, "true");
	} else {
		input.putback(c);
		return LoadNumber(input);
	}
}

Document Load(istream& input) {
	return Document{LoadNode(input)};
}

}
