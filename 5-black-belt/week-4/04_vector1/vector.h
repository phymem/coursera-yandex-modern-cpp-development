#pragma once

#include <cstddef>
#include <memory>
#include <algorithm>

template <typename T>
class Vector {
public:
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
