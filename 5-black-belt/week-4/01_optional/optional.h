#include <utility>

// Исключение этого типа должно генерироваться при обращении к "пустому" Optional в функции Value
struct BadOptionalAccess {
};

template <typename T>
class Optional {
private:
	// alignas нужен для правильного выравнивания блока памяти
	alignas(T) unsigned char data[sizeof(T)];
	T* value = nullptr;

public:
	Optional() = default;
	Optional(const T& elem)
		: value(new(data) T{elem}) {}
	Optional(T&& elem)
		: value(new(data) T{std::move(elem)}) {}
	Optional(const Optional& other)
		: value(other.value ? new(data) T{*other.value} : nullptr) {}
	Optional(Optional&& other)
		: value(other.value ? new(data) T{std::move(*other.value)} : nullptr) {}

	Optional& operator=(const T& elem) {
		if (value) {
			*value = elem;
		}
		else {
			value = new(data) T{elem};
		}
		return *this;
	}
	Optional& operator=(T&& elem) {
		if (value) {
			*value = std::move(elem);
		}
		else {
			value = new(data) T{std::move(elem)};
		}
		return *this;
	}
	Optional& operator=(const Optional& other) {
		if (this != &other) {
			if (value && other.value) {
				*value = *other.value;
			}
			else {
				Reset();
				if (other.value) {
					value = new(data) T{*other.value};
				}
			}
		}
		return *this;
	}
	Optional& operator=(Optional&& other) {
		if (value && other.value) {
			*value = std::move(*other.value);
		}
		else {
			Reset();
			if (other.value) {
				value = new(data) T{std::move(*other.value)};
			}
		}
		return *this;
	}

	bool HasValue() const { return value; }

	// Эти операторы не должны делать никаких проверок на пустоту.
	// Проверки остаются на совести программиста.
	T& operator*() { return *value; }
	const T& operator*() const { return *value; }
	T* operator->() { return value; }
	const T* operator->() const { return value; }

	// Генерирует исключение BadOptionalAccess, если объекта нет
	T& Value() {
		if (!value) {
			throw BadOptionalAccess{};
		}
		return *value;
	}
	const T& Value() const {
		if (!value) {
			throw BadOptionalAccess{};
		}
		return *value;
	}

	void Reset() {
		if (value) {
			value->~T();
			value = nullptr;
		}
	}

	~Optional() { Reset(); }
};
