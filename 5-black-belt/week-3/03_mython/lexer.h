#pragma once

#include <iosfwd>
#include <string>
#include <sstream>
#include <variant>
#include <stdexcept>
#include <optional>

class TestRunner;

namespace Parse {

namespace TokenType {
	struct Number {
		int value;
	};

	struct Id {
		std::string value;
	};

	struct Char {
		char value;
	};

	struct String {
		std::string value;
	};

	struct Class{};
	struct Return{};
	struct If {};
	struct Else {};
	struct Def {};
	struct Newline {};
	struct Print {};
	struct Indent {};
	struct Dedent {};
	struct Eof {};
	struct And {};
	struct Or {};
	struct Not {};
	struct Eq {};
	struct NotEq {};
	struct LessOrEq {};
	struct GreaterOrEq {};
	struct None {};
	struct True {};
	struct False {};
}

using TokenBase = std::variant<
	TokenType::Number,
	TokenType::Id,
	TokenType::Char,
	TokenType::String,
	TokenType::Class,
	TokenType::Return,
	TokenType::If,
	TokenType::Else,
	TokenType::Def,
	TokenType::Newline,
	TokenType::Print,
	TokenType::Indent,
	TokenType::Dedent,
	TokenType::And,
	TokenType::Or,
	TokenType::Not,
	TokenType::Eq,
	TokenType::NotEq,
	TokenType::LessOrEq,
	TokenType::GreaterOrEq,
	TokenType::None,
	TokenType::True,
	TokenType::False,
	TokenType::Eof
>;


//По сравнению с условием задачи мы добавили в тип Token несколько
//удобных методов, которые делают код короче. Например,
//
//token.Is<TokenType::Id>()
//
//гораздо короче, чем
//
//std::holds_alternative<TokenType::Id>(token).
struct Token : TokenBase {
	using TokenBase::TokenBase;

	template <typename T>
	bool Is() const {
		return std::holds_alternative<T>(*this);
	}

	template <typename T>
	const T& As() const {
		return std::get<T>(*this);
	}

	template <typename T>
	const T* TryAs() const {
		return std::get_if<T>(this);
	}
};

bool operator == (const Token& lhs, const Token& rhs);
std::ostream& operator << (std::ostream& os, const Token& rhs);

class LexerError : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

class Lexer {
public:
	explicit Lexer(std::istream& input) :
		start_line_(true),
		empty_line_(true),
		space_size_(0),
		tab_size_(0),
		tab_num_(0),
		input_(input) { NextToken(); }

	const Token& CurrentToken() const {
		return current_;
	}
	Token NextToken() {
		current_ = NextTokenInternal();
		return current_;
	}

	template <typename T>
	const T& Expect() const {
		if (current_.Is<T>()) return current_.As<T>();
		throw LexerError{"Lexer::Expect() error"};
	}

	template <typename T, typename U>
	void Expect(const U& value) const {
		if (current_.Is<T>() && current_ == Token(T{value}))
			return;
		throw LexerError{"Lexer::Expect(value) error"};
	}

	template <typename T>
	const T& ExpectNext() {
		NextToken();
		return Expect<T>();
	}

	template <typename T, typename U>
	void ExpectNext(const U& value) {
		NextToken();
		Expect<T, U>(value);
	}

private:

	Token ParseDigit();
	Token ParseIdent();
	Token ParsePunct();
	Token ParseString();

	Token NextTokenInternal();

	bool start_line_;
	bool empty_line_;
	int space_size_;
	int tab_size_;
	int tab_num_;

	Token current_;
	std::istream& input_;
};

void RunLexerTests(TestRunner& test_runner);

} /* namespace Parse */
