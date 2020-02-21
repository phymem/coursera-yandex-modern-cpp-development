#include <list>
#include <string>
#include <cassert>
#include <algorithm>
#include "test_runner.h"
using namespace std;

class Editor {
public:

	Editor() {
		m_cursor = m_text.begin();
	}

	void Left() {
		m_cursor = DecCursor(m_cursor);
	}

	void Right() {
		m_cursor = IncCursor(m_cursor);
	}

	void Insert(char token) {
		m_text.insert(m_cursor, token);
	}

	void Cut(size_t tokens = 1) {
		auto cut_end = IncCursor(m_cursor, tokens);
		m_clipboard.assign(m_cursor, cut_end);
		m_cursor = m_text.erase(m_cursor, cut_end);
	}

	void Copy(size_t tokens = 1) {
		m_clipboard.assign(m_cursor, IncCursor(m_cursor, tokens));
	}

	void Paste() {
		m_text.insert(m_cursor, begin(m_clipboard), end(m_clipboard));
	}

	string GetText() const {
		return string(m_text.begin(), m_text.end());
	}

private:

	list<char>::iterator IncCursor(list<char>::iterator it, size_t distance = 1) const {
		for (; distance-- && it != end(m_text); ++it);
		return it;
	}
	list<char>::iterator DecCursor(list<char>::iterator it, size_t distance = 1) const {
		for (; distance-- && it != begin(m_text); --it);
		return it;
	}

private:

	list<char>::iterator m_cursor;

	list<char> m_text;
	list<char> m_clipboard;
};

void TypeText(Editor& editor, const string& text) {
	for(char c : text) {
		editor.Insert(c);
	}
}

void TestEditing() {
	{
		Editor editor;

		const size_t text_len = 12;
		TypeText(editor, "hello, world");

		ASSERT_EQUAL(editor.GetText(), "hello, world");
	}
	{
		Editor editor;

		const size_t text_len = 12;
		TypeText(editor, "hello, world");

		for (unsigned int i = 0; i < text_len; ++i)
			editor.Left();

		editor.Copy(7);
		editor.Paste();

		ASSERT_EQUAL(editor.GetText(), "hello, hello, world");
	}
	{
		Editor editor;

		const size_t text_len = 12;
		const size_t first_part_len = 7;
		TypeText(editor, "hello, world");
		for(size_t i = 0; i < text_len; ++i) {
			editor.Left();
		}
		editor.Cut(first_part_len);
		for(size_t i = 0; i < text_len - first_part_len; ++i) {
			editor.Right();
		}
		TypeText(editor, ", ");
		editor.Paste();
		editor.Left();
		editor.Left();
		editor.Cut(3);

		ASSERT_EQUAL(editor.GetText(), "world, hello");
	}
	{
		Editor editor;

		TypeText(editor, "misprnit");
		editor.Left();
		editor.Left();
		editor.Left();
		editor.Cut(1);
		editor.Right();
		editor.Paste();

		ASSERT_EQUAL(editor.GetText(), "misprint");
	}
	{
		Editor editor;

		TypeText(editor, "test test test");

		for (unsigned int i = 0; i < 15; ++i)
			editor.Left();

		editor.Cut(5);
		ASSERT_EQUAL(editor.GetText(), "test test");

		editor.Paste();
		ASSERT_EQUAL(editor.GetText(), "test test test");

		editor.Left();
		editor.Left();
		editor.Left();
		editor.Left();
		editor.Left();
		editor.Cut(5);
		editor.Cut(5);
		ASSERT_EQUAL(editor.GetText(), "test");

		editor.Paste();
		ASSERT_EQUAL(editor.GetText(), "test test");

		editor.Left();
		editor.Left();
		editor.Left();
		editor.Left();
		editor.Left();
		editor.Cut(5);
		editor.Cut(5);
		editor.Cut(5);
		ASSERT_EQUAL(editor.GetText(), "");

		editor.Paste();
		ASSERT_EQUAL(editor.GetText(), "");
	}
}

void TestReverse() {
	Editor editor;

	const string text = "esreveR";
	for(char c : text) {
		editor.Insert(c);
		editor.Left();
	}

	ASSERT_EQUAL(editor.GetText(), "Reverse");
}

void TestNoText() {
	Editor editor;
	ASSERT_EQUAL(editor.GetText(), "");

	editor.Left();
	editor.Left();
	editor.Right();
	editor.Right();
	editor.Copy(0);
	editor.Cut(0);
	editor.Paste();

	ASSERT_EQUAL(editor.GetText(), "");
}

void TestEmptyBuffer() {
	Editor editor;

	editor.Paste();
	TypeText(editor, "example");
	editor.Left();
	editor.Left();
	editor.Paste();
	editor.Right();
	editor.Paste();
	editor.Copy(0);
	editor.Paste();
	editor.Left();
	editor.Cut(0);
	editor.Paste();

	ASSERT_EQUAL(editor.GetText(), "example");
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestEditing);
	RUN_TEST(tr, TestReverse);
	RUN_TEST(tr, TestNoText);
	RUN_TEST(tr, TestEmptyBuffer);
	return 0;
}
