#pragma once

#include <stdexcept>
#include <array>
using namespace std;

template <typename T, size_t N>
class StackVector {
public:

	explicit StackVector(size_t a_size = 0) :
		m_size(a_size)
	{
		if (m_size > N)
			throw invalid_argument("StackVector");
	}

	T& operator[](size_t index) { return m_data[index]; }
	const T& operator[](size_t index) const { return m_data[index]; }

	T* begin() { return &m_data[0]; }
	T* end() { return &m_data[0] + m_size; }
	const T* begin() const { return &m_data[0]; }
	const T* end() const { return &m_data[0] + m_size; }

	size_t Size() const { return m_size; }
	size_t Capacity() const { return N; }

	void PushBack(const T& value) {
		if (m_size == N)
			throw overflow_error("StackVector");

		m_data[m_size++] = value;
	}
	T PopBack() {
		if (!m_size)
			throw underflow_error("StackVector");
		return m_data[--m_size];
	}

private:

	size_t m_size;
	array<T, N> m_data;
};

