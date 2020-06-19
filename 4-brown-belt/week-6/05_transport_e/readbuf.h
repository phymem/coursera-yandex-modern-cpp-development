#pragma once

#include <stdexcept>
#include <string>
#include <cctype>

char skip_space() {
	char ch;
	while (isspace(ch = getchar())) {}
	return ch;
}

struct ReadBuf {
	char* m_buf;
	size_t m_size;
	size_t m_len;

	ReadBuf(char* f_buf, size_t f_size) :
		m_buf(f_buf), m_size(f_size), m_len(0) { *m_buf = '\0'; }

	void clear() {
		m_buf[m_len = 0] = '\0';
	}

	template <typename predicate>
	ReadBuf& read(predicate f_pred, bool f_skip_space = true) {
		clear();
		char ch;
		if (f_skip_space) {
			ch = skip_space();
			if (f_pred(ch)) {
				m_buf[m_len++] = ch;
			}
			else {
				if (ch != EOF)
					ungetc(ch, stdin);
				return *this;
			}
		}
		while (f_pred(ch = getchar())) {
			m_buf[m_len++] = ch;
			if (m_len >= m_size)
				throw std::invalid_argument("read_buf: buffer overflow");
		}
		m_buf[m_len] = '\0';
		if (ch != EOF)
			ungetc(ch, stdin);
		return *this;
	}

	ReadBuf& readString() {
		clear();
		char ch = skip_space();
		if (ch != '"')
			throw std::invalid_argument("readString: '\"'");
		while ((ch = getchar()) != '"') {
			m_buf[m_len++] = ch;
			if (m_len >= m_size)
				throw std::invalid_argument("readString: buffer overflow");
		}
		m_buf[m_len] = '\0';
		return *this;
	}

	ReadBuf& readNumber() {
		clear();
		char ch = skip_space();
		if (ch != '+' && ch != '-' && ch != '.' && !isdigit(ch))
			throw std::invalid_argument("readNumber: '+' '-' '.' digit");
		m_buf[m_len++] = ch;
		if (ch != '.') {
			while (isdigit(ch = getchar())) {
				m_buf[m_len++] = ch;
				if (m_len >= m_size)
					throw std::invalid_argument("readNumber: buffer overflow");
			}
			if (ch == '.') {
				m_buf[m_len++] = ch;
				if (m_len >= m_size)
					throw std::invalid_argument("readNumber: buffer overflow");
			}
		}
		if (ch == '.') {
			while (isdigit(ch = getchar())) {
				m_buf[m_len++] = ch;
				if (m_len >= m_size)
					throw std::invalid_argument("readNumber: buffer overflow");
			}
		}
		m_buf[m_len] = '\0';
		if (ch != EOF)
			ungetc(ch, stdin);
		return *this;
	}

	const char* to_c_str() const  { return m_buf; }

	std::string_view to_string_view() const { return std::string_view(m_buf, m_len); }

	int to_int() const { return atoi(m_buf); }

	double to_double() const { return std::strtod(m_buf, nullptr); }
};
