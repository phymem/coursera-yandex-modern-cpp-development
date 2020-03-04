#pragma once

#include <algorithm>
#include <utility>

using namespace std;

template <typename T>
class SimpleVector {
public:

	SimpleVector() :
		m_data(nullptr),
		m_size(0),
		m_capacity(0) {}

	explicit SimpleVector(size_t size) :
		m_data(new T[size]),
		m_size(size),
		m_capacity(size) {}

	~SimpleVector() {
		delete [] m_data;
	}

	T& operator[](size_t index) {
		return m_data[index];
	}

	T* begin() { return m_data; }
	T* end() { return m_data + m_size; }

	const T* begin() const { return m_data; }
	const T* end() const { return m_data + m_size; }

	size_t Size() const { return m_size; }
	size_t Capacity() const { return m_capacity; }

	void PushBack(T value) {
		if (m_size >= m_capacity) {
			ExpandVector();
		}
		m_data[m_size++] = std::move(value);
	}

private:

	void ExpandVector() {
		if (m_capacity)
			m_capacity *= 2;
		else
			m_capacity = 1;
		T* new_data = new T[m_capacity];
		move(&m_data[0], &m_data[m_size], new_data);
		delete [] m_data;
		m_data = new_data;
	}

private:

	T* m_data;

	size_t m_size;
	size_t m_capacity;
};
