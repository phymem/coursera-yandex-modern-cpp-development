#pragma once

#include <cstdlib>

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

	size_t Size() const { return m_size; }
	size_t Capacity() const { return m_capacity; }

	void PushBack(const T& value) {
		if (m_size >= m_capacity) {
			if (m_capacity)
				m_capacity *= 2;
			else
				m_capacity = 1;
			T* new_data = new T[m_capacity];
			for (unsigned int i = 0; i < m_size; ++i) {
				new_data[i] = m_data[i];
			}
			delete [] m_data;
			m_data = new_data;
		}
		m_data[m_size++] = value;
	}

private:

	T* m_data;

	size_t m_size;
	size_t m_capacity;
};
