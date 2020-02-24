#pragma once

#include <algorithm>
#include <utility>

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

	SimpleVector(const SimpleVector<T>& vec) :
		m_data(vec.m_data ? new T[vec.m_size] : nullptr),
		m_size(vec.m_size),
		m_capacity(vec.m_size)
	{ std::copy(vec.begin(), vec.end(), begin()); }

	~SimpleVector() {
		delete [] m_data;
	}

	SimpleVector<T>& operator = (const SimpleVector<T>& vec) {
		if (this != &vec) {
			if (m_capacity > vec.m_size) {
				std::copy(vec.begin(), vec.end(), begin());
				m_size = vec.m_size;
			}
			else {
				SimpleVector<T> tmp(vec);
				std::swap(m_data, tmp.m_data);
				std::swap(m_size, tmp.m_size);
				std::swap(m_capacity, tmp.m_capacity);
			}
		}
		return *this;
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
