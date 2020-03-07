#include "test_runner.h"
#include <numeric>
#include <vector>
#include <future>

using namespace std;

int64_t CalculateMatrixSum(const vector<vector<int>>& matrix) {
	const int NUM_THREADS = 8;
	unsigned int step = (matrix.size() + NUM_THREADS - 1) / NUM_THREADS;
	unsigned int first = 0;

	vector<future<int64_t>> partial_sums;
	while (first < matrix.size()) {
		unsigned int last = min<unsigned int>(first + step, matrix.size());
		partial_sums.push_back(async(
			[first, last, &matrix]() {
				int64_t sum = 0;
				for (int i = first; i < last; ++i) {
					sum = accumulate(begin(matrix[i]), end(matrix[i]), sum);
				}
				return sum;
			}
		));
		first = last;
	}

	int64_t ret = 0;
	for (auto& s : partial_sums) {
		ret += s.get();
	}

	return ret;
}

void TestCalculateMatrixSum() {
	const vector<vector<int>> matrix = {
		{1, 2, 3, 4},
		{5, 6, 7, 8},
		{9, 10, 11, 12},
		{13, 14, 15, 16}
	};
	ASSERT_EQUAL(CalculateMatrixSum(matrix), 136);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestCalculateMatrixSum);
}
