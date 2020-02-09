#include "test_runner.h"

#include <vector>

using namespace std;

template <typename T>
class Table {
public:

	Table(size_t rows,  size_t columns) :
		m_table(rows * columns),
		m_num_rows(rows),
		m_num_columns(columns) {}

	class Column {
	public:

		Column(vector<T>& tbl, size_t offset) :
			m_table(tbl),
			m_offset(offset) {}

		T& operator[] (size_t column) {
			return m_table[m_offset + column];
		}
		const T& operator[] (size_t column) const {
			return m_table[m_offset + column];
		}

	private:

		vector<T>& m_table;

		size_t m_offset;
	};

	Column operator[] (size_t row) {
		return Column(m_table, row * m_num_columns);
	}
	const Column operator[] (size_t row) const {
		return Column(m_table, row * m_num_columns);
	}

	pair<size_t, size_t> Size() const {
		return make_pair(m_num_rows, m_num_columns);
	}

	void Resize(size_t new_rows, size_t new_columns) {
		vector<T> new_table(new_rows * new_columns);

		size_t rows = min(new_rows, m_num_rows);
		size_t cols = min(new_columns, m_num_columns);

		for (size_t r = 0; r < rows; ++r) {
			for (size_t c = 0; c < cols; ++c) {
				new_table[r * new_columns + c] = m_table[r * m_num_columns + c];
			}
		}

		m_table = new_table;
		m_num_rows = new_rows;
		m_num_columns = new_columns;
	}

private:

	vector<T> m_table;

	size_t m_num_rows;
	size_t m_num_columns;
};

void TestTable() {
	Table<int> t(1, 1);
	ASSERT_EQUAL(t.Size().first, 1u);
	ASSERT_EQUAL(t.Size().second, 1u);
	t[0][0] = 42;
	ASSERT_EQUAL(t[0][0], 42);
	t.Resize(3, 4);
	ASSERT_EQUAL(t.Size().first, 3u);
	ASSERT_EQUAL(t.Size().second, 4u);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestTable);
	return 0;
}
