#include <map>
#include <iostream>
#include <string>
#include <vector>

// Serialization

template <typename T>
inline void Serialize(T pod, std::ostream& out) {
	out.write(reinterpret_cast<const char*>(&pod), sizeof(pod));
}

inline void Serialize(const std::string& str, std::ostream& out) {
	Serialize(str.size(), out);
	out.write(str.c_str(), str.size());
}

template <typename T1, typename T2>
void Serialize(const std::map<T1, T2>& data, std::ostream& out);

template <typename T>
inline void Serialize(const std::vector<T>& data, std::ostream& out) {
	Serialize(data.size(), out);
	for (const T& val : data) {
		Serialize(val, out);
	}
}

template <typename T1, typename T2>
inline void Serialize(const std::map<T1, T2>& data, std::ostream& out) {
	Serialize(data.size(), out);
	for (const auto& p : data) {
		Serialize(p.first, out);
		Serialize(p.second, out);
	}
}

// Deserialization

template <typename T>
inline void Deserialize(std::istream& in, T& pod) {
	in.read(reinterpret_cast<char*>(&pod), sizeof(pod));
}

inline void Deserialize(std::istream& in, std::string& str) {
	size_t n;
	Deserialize(in, n);
	str.resize(n);
	in.read(str.data(), n);
}

template <typename T1, typename T2>
void Deserialize(std::istream& in, std::map<T1, T2>& data);

template <typename T>
inline void Deserialize(std::istream& in, std::vector<T>& data) {
	size_t n;
	Deserialize(in, n);
	data.reserve(n);
	for (size_t i = 0; i < n; ++i) {
		T val;
		Deserialize(in, val);
		data.push_back(std::move(val));
	}
}

template <typename T1, typename T2>
inline void Deserialize(std::istream& in, std::map<T1, T2>& data) {
	size_t n;
	Deserialize(in, n);
	for (size_t i = 0; i < n; ++i) {
		T1 key;
		T2 val;
		Deserialize(in, key);
		Deserialize(in, val);
		data.insert(
			std::make_pair(
				std::move(key),
				std::move(val)
			)
		);
	}
}
