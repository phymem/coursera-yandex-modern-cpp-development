#include "lexer.h"

#include <cassert>
#include <algorithm>
#include <charconv>
#include <unordered_map>

using namespace std;

namespace Parse {

static bool is_ident(char ch) {
	return ch == '_' || isalnum(ch);
}

static bool is_space(char ch) {
	return ch == ' ' || ch == '\t';
}

static bool is_punct(char ch) {
	switch (ch) {
	case '=':
	case '<':
	case '>':
	case '!':
	case '+':
	case '-':
	case '*':
	case '/':
	case ',':
	case ':':
	case '(':
	case ')':
	case '{':
	case '}':
	case '.':
	case '?':
		return true;
	default: ;
	}
	return false;
}

bool operator == (const Token& lhs, const Token& rhs) {
	using namespace TokenType;

	if (lhs.index() != rhs.index()) {
		return false;
	}
	if (lhs.Is<Char>()) {
		return lhs.As<Char>().value == rhs.As<Char>().value;
	} else if (lhs.Is<Number>()) {
		return lhs.As<Number>().value == rhs.As<Number>().value;
	} else if (lhs.Is<String>()) {
		return lhs.As<String>().value == rhs.As<String>().value;
	} else if (lhs.Is<Id>()) {
		return lhs.As<Id>().value == rhs.As<Id>().value;
	} else {
		return true;
	}
}

std::ostream& operator << (std::ostream& os, const Token& rhs) {
	using namespace TokenType;

#define VALUED_OUTPUT(type) \
	if (auto p = rhs.TryAs<type>()) return os << #type << '{' << p->value << '}';

	VALUED_OUTPUT(Number);
	VALUED_OUTPUT(Id);
	VALUED_OUTPUT(String);
	VALUED_OUTPUT(Char);

#undef VALUED_OUTPUT

#define UNVALUED_OUTPUT(type) \
	if (rhs.Is<type>()) return os << #type;

	UNVALUED_OUTPUT(Class);
	UNVALUED_OUTPUT(Return);
	UNVALUED_OUTPUT(If);
	UNVALUED_OUTPUT(Else);
	UNVALUED_OUTPUT(Def);
	UNVALUED_OUTPUT(Newline);
	UNVALUED_OUTPUT(Print);
	UNVALUED_OUTPUT(Indent);
	UNVALUED_OUTPUT(Dedent);
	UNVALUED_OUTPUT(And);
	UNVALUED_OUTPUT(Or);
	UNVALUED_OUTPUT(Not);
	UNVALUED_OUTPUT(Eq);
	UNVALUED_OUTPUT(NotEq);
	UNVALUED_OUTPUT(LessOrEq);
	UNVALUED_OUTPUT(GreaterOrEq);
	UNVALUED_OUTPUT(None);
	UNVALUED_OUTPUT(True);
	UNVALUED_OUTPUT(False);
	UNVALUED_OUTPUT(Eof);

#undef UNVALUED_OUTPUT

	return os << "Unknown token :(";
}

Token Lexer::ParseIdent() {
	string lex;
	while (is_ident(input_.peek())) {
		lex += input_.get();
	}

	if (lex == "class") return TokenType::Class{};
	if (lex == "return") return TokenType::Return{};
	if (lex == "if") return TokenType::If{};
	if (lex == "else") return TokenType::Else{};
	if (lex == "def") return TokenType::Def{};
	if (lex == "print") return TokenType::Print{};
	if (lex == "and") return TokenType::And{};
	if (lex == "or") return TokenType::Or{};
	if (lex == "not") return TokenType::Not{};
	if (lex == "None") return TokenType::None{};
	if (lex == "True") return TokenType::True{};
	if (lex == "False") return TokenType::False{};

	return TokenType::Id{ lex };
}

Token Lexer::ParsePunct() {
	char ch = input_.get();
	switch (ch) {
	case '=':
	case '!':
	case '<':
	case '>':
		if (input_.peek() == '=') {
			input_.get();
			switch (ch) {
			case '=': return TokenType::Eq{};
			case '!': return TokenType::NotEq{};
			case '<': return TokenType::LessOrEq{};
			case '>': return TokenType::GreaterOrEq{};
			default:
				assert(!"ParsePunct - this should never be called");
			}
		}
		break;
	default: ;
	}

	return TokenType::Char{ ch };
}

Token Lexer::ParseDigit() {
	string lex;
	while (isdigit(input_.peek())) {
		lex += input_.get();
	}

	int val;
	const char* ret  = from_chars(lex.data(), lex.data() + lex.size(), val).ptr;
	assert(ret != lex.data());

	return TokenType::Number{ val };
}

Token Lexer::ParseString() {
	string lex;
	char ch;
	char quote_sym = input_.get();
	while ((ch = input_.get()) != quote_sym) {
		lex += ch;
	}

	return TokenType::String{ lex };
}

Token Lexer::NextTokenInternal() {
	char ch;
	while (true) {
		ch = input_.peek();
		if (start_line_) {
			if (is_space(ch)) {
				space_size_ = 0;
				while (is_space(ch = input_.peek())) {
					input_.get();
					space_size_++;
				}
			}
			if (ch == '\n') {
				input_.get();
				space_size_ = 0;
				continue;
			}
			if (space_size_ > tab_size_ * tab_num_) {
				if (!tab_num_) tab_size_ = space_size_;
				tab_num_++;
				return TokenType::Indent{};
			}
			if (space_size_ < tab_size_ * tab_num_) {
				tab_num_--;
				if (!tab_num_) tab_size_ = 0;
				return TokenType::Dedent{};
			}
			start_line_ = false;
		}

		if (ch == EOF) {
			// emit Newline at the end of nonempty line
			if (!empty_line_) {
				empty_line_ = true;
				return TokenType::Newline{};
			}
			break;
		}

		if (ch == '\n') {
			input_.get();
			space_size_ = 0;
			start_line_ = true;
			empty_line_ = true;
			return TokenType::Newline{};
		}

		Token tok = TokenType::Eof{};
		if (ch == '\'' || ch == '"') {
			tok = ParseString();
		}
		else if (isdigit(ch)) {
			tok = ParseDigit();
		}
		else if (is_punct(ch)) {
			tok = ParsePunct();
		}
		else if (is_ident(ch)) {
			tok = ParseIdent();
		}
		else {
			assert(!"this should never be called");
		}

		while (is_space(input_.peek())) {
			input_.get();
		}

		empty_line_ = false;
		return tok;
	}

	return TokenType::Eof{};
}

} /* namespace Parse */
