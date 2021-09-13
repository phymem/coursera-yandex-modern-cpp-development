#pragma once

#include <cassert>
#include <cstddef>
#include <memory>
#include <algorithm>

template <typename T>
class Vector {
public:

	//
	// vector1
	//

	Vector() {
		size_ = 0;
	}
	Vector(size_t n) : mem_(n) {
		std::uninitialized_value_construct_n(mem_.data_, n);
		size_ = n;
	}
	Vector(const Vector& other) : mem_(other.size_) {
		std::uninitialized_copy_n(
			other.mem_.data_, other.size_, mem_.data_
		);
		size_ = other.size_;
	}
	Vector(Vector&& other) noexcept {
		size_ = 0;
		Swap(other);
	}

	~Vector() {
		std::destroy_n(mem_.data_, size_);
	}

	Vector& operator = (const Vector& other) {
		if (this != &other) {
			if (mem_.capacity_ < other.size_) {
				Vector tmp(other);
				Swap(tmp);
			}
			else {
				size_t min_size = std::min(size_, other.size_);
				std::copy(
					other.mem_.data_,
					other.mem_.data_ + min_size,
					mem_.data_
				);
				if (min_size < size_) {
					std::destroy_n(
						mem_.data_ + min_size,
						size_ - min_size
					);
				}
				else if (min_size < other.size_) {
					std::uninitialized_copy_n(
						other.mem_.data_ + min_size,
						other.size_ - min_size,
						mem_.data_ + min_size
					);
				}
				size_ = other.size_;
			}
		}
		return *this;
	}
	Vector& operator = (Vector&& other) noexcept {
		Swap(other);
		return *this;
	}

	void Reserve(size_t n) {
		if (mem_.capacity_ < n) {
			RawMemory new_mem(n);
			std::uninitialized_move_n(
				mem_.data_, size_, new_mem.data_
			);
			std::destroy_n(mem_.data_, size_);
			mem_.Swap(new_mem);
		}
	}

	void Resize(size_t n) {
		Reserve(n);
		if (size_ < n) {
			std::uninitialized_value_construct_n(
				mem_.data_ + size_, n - size_
			);
		}
		else if (size_ > n) {
			std::destroy_n(mem_.data_ + n, size_ - n);
		}
		size_ = n;
	}

	void PushBack(const T& elem) {
		if (size_ == mem_.capacity_) {
			Reserve(size_ ? 2 * size_ : 1);
		}
		new (mem_.data_ + size_) T(elem);
		size_++;
	}
	void PushBack(T&& elem) {
		if (size_ == mem_.capacity_) {
			Reserve(size_ ? 2 * size_ : 1);
		}
		new (mem_.data_ + size_) T(std::move(elem));
		size_++;
	}

	template <typename ... Args>
	T& EmplaceBack(Args&&... args) {
		if (size_ == mem_.capacity_) {
			Reserve(size_ ? 2 * size_ : 1);
		}
		T* p = new (mem_.data_ + size_) T(std::forward<Args>(args)...);
		size_++;
		return *p;
	}

	void PopBack() {
		std::destroy_at(mem_.data_ + size_ - 1);
		--size_;
	}

	size_t Size() const noexcept {
		return size_;
	}

	size_t Capacity() const noexcept {
		return mem_.capacity_;
	}

	const T& operator[](size_t i) const {
		return mem_.data_[i];
	}
	T& operator[](size_t i) {
		return mem_.data_[i];
	}

	//
	// vector2
	//

	using iterator = T*;
	using const_iterator = const T*;

	iterator begin() noexcept {
		return mem_.data_;
	}
	iterator end() noexcept {
		return mem_.data_ + size_;
	}

	const_iterator begin() const noexcept {
		return mem_.data_;
	}
	const_iterator end() const noexcept {
		return mem_.data_ + size_;
	}

	// Тут должна быть такая же реализация, как и для константных версий begin/end
	const_iterator cbegin() const noexcept {
		return mem_.data_;
	}
	const_iterator cend() const noexcept {
		return mem_.data_ + size_;
	}

	// Вставляет элемент перед pos
	// Возвращает итератор на вставленный элемент
	iterator Insert(const_iterator pos, const T& elem) {
		size_t pos_ndx = pos - mem_.data_;
		PushBack(elem);
		std::rotate(
			mem_.data_ + pos_ndx,
			mem_.data_ + size_ - 1,
			mem_.data_ + size_
		);
		return mem_.data_ + pos_ndx;
	}
	iterator Insert(const_iterator pos, T&& elem) {
		size_t pos_ndx = pos - mem_.data_;
		PushBack(std::move(elem));
		std::rotate(
			mem_.data_ + pos_ndx,
			mem_.data_ + size_ - 1,
			mem_.data_ + size_
		);
		return mem_.data_ + pos_ndx;
	}

	// Конструирует элемент по заданным аргументам конструктора перед pos
	// Возвращает итератор на вставленный элемент
	template <typename ... Args>
	iterator Emplace(const_iterator pos, Args&&... args) {
		size_t pos_ndx = pos - mem_.data_;
		EmplaceBack(std::forward<Args>(args)...);
		std::rotate(
			mem_.data_ + pos_ndx,
			mem_.data_ + size_ - 1,
			mem_.data_ + size_
		);
		return mem_.data_ + pos_ndx;
	}

	// Удаляет элемент на позиции pos
	// Возвращает итератор на элемент, следующий за удалённым
	iterator Erase(const_iterator pos) {
		size_t pos_ndx = pos - mem_.data_;
		std::move(
			mem_.data_ + pos_ndx + 1,
			mem_.data_ + size_,
			mem_.data_ + pos_ndx
		);
		PopBack();
		return mem_.data_ + pos_ndx;
	}

private:

	void Swap(Vector& other) noexcept {
		mem_.Swap(other.mem_);
		std::swap(size_, other.size_);
	}

	struct RawMemory {
		size_t capacity_;
		T* data_;

		RawMemory()
			: capacity_(0)
			, data_(nullptr) {}

		RawMemory(size_t capacity)
			: capacity_(capacity)
			, data_(static_cast<T*>(operator new(capacity * sizeof(T)))) {}

		~RawMemory() {
			operator delete(data_);
		}

		void Swap(RawMemory& other) noexcept {
			std::swap(capacity_, other.capacity_);
			std::swap(data_, other.data_);
		}
	};

	RawMemory mem_;
	size_t size_;
};
