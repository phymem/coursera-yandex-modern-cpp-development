#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>

using namespace std;

class Matrix {
public:

	Matrix() :
		m_num_rows(0),
		m_num_cols(0) {}

	~Matrix() {}

	Matrix(int num_rows, int num_cols) {
		Reset(num_rows, num_cols);
	}

	void Reset(int num_rows, int num_cols) {
		if (num_rows < 0)
			throw out_of_range("negative num_rows");
		if (num_cols < 0)
			throw out_of_range("negative num_cols");

		if (num_rows == 0 || num_cols == 0)
			num_rows = num_cols = 0;

		m_num_rows = num_rows;
		m_num_cols = num_cols;

		m_matrix.assign(num_rows * num_cols, 0);
	}

	int At(int row, int col) const {
		if (row < 0 || row >= m_num_rows)
			throw out_of_range("At: row is out of range");
		if (col < 0 || col >= m_num_cols)
			throw out_of_range("At: col is out of range");

		return m_matrix[row * m_num_cols + col];
	}
	int& At(int row, int col) {
		if (row < 0 || row >= m_num_rows)
			throw out_of_range("At: row is out of range");
		if (col < 0 || col >= m_num_cols)
			throw out_of_range("At: col is out of range");

		return m_matrix[row * m_num_cols + col];
	}

	int GetNumRows() const { return m_num_rows; }
	int GetNumColumns() const { return m_num_cols; }

private:

	vector<int> m_matrix;

	int m_num_rows;
	int m_num_cols;
};

Matrix operator + (const Matrix& a, const Matrix& b) {
	if (a.GetNumRows() != b.GetNumRows())
		throw invalid_argument("operator +: number of rows doesn't match");
	if (a.GetNumColumns() != b.GetNumColumns())
		throw invalid_argument("operator +: number of columns doesn't match");

	Matrix ret(a.GetNumRows(), a.GetNumColumns());
	for (unsigned int row = 0; row < a.GetNumRows(); ++row) {
		for (unsigned int col = 0; col < a.GetNumColumns(); ++col) {
			ret.At(row, col) = a.At(row, col) + b.At(row, col);
		}
	}

	return ret;
}

bool operator == (const Matrix& a, const Matrix& b) {
	if (a.GetNumRows() != b.GetNumRows() || a.GetNumColumns() != b.GetNumColumns())
		return false;

	for (unsigned int row = 0; row < a.GetNumRows(); ++row) {
		for (unsigned int col = 0; col < a.GetNumColumns(); ++col) {
			if (a.At(row, col) != b.At(row, col))
				return false;
		}
	}

	return true;
}

istream& operator >> (istream& input, Matrix& matrix) {
	int num_rows, num_cols;
	input >> num_rows >> num_cols;
	matrix.Reset(num_rows, num_cols);

	for (unsigned int row = 0; row < num_rows; ++row) {
		for (unsigned int col = 0; col < num_cols; ++col) {
			input >> matrix.At(row, col);
		}
	}

	return input;
}
ostream& operator << (ostream& output, const Matrix& matrix) {
	output << matrix.GetNumRows() << ' ' << matrix.GetNumColumns() << endl;

	for (unsigned int row = 0; row < matrix.GetNumRows(); ++row) {
		for (unsigned int col = 0; col < matrix.GetNumColumns(); ++col) {
			if (col) output << ' ';
			output << matrix.At(row, col);
		}
		output << endl;
	}

	return output;
}

int main() {
	Matrix one;
	Matrix two;

	cin >> one >> two;
	cout << one + two << endl;
	return 0;
}
