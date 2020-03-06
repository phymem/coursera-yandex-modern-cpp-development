#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <utility>
#include <vector>

#include <map>

using namespace std;

template <typename T>
class PriorityCollection {
public:

	PriorityCollection() : m_next_id(1) {}

	using Id = unsigned long long;

	// Добавить объект с нулевым приоритетом
	// с помощью перемещения и вернуть его идентификатор
	Id Add(T object) {
		Id id = m_next_id++;
		m_items[id] = MapItem(move(object));
		m_priority_set.insert(SetKey(id, 0));
		return id;
	}

	// Добавить все элементы диапазона [range_begin, range_end)
	// с помощью перемещения, записав выданные им идентификаторы
	// в диапазон [ids_begin, ...)
	template <typename ObjInputIt, typename IdOutputIt>
	void Add(ObjInputIt range_begin, ObjInputIt range_end, IdOutputIt ids_begin) {
		while (range_begin != range_end) {
			*ids_begin++ = Add(move(*range_begin++));
		}
	}

	// Определить, принадлежит ли идентификатор какому-либо
	// хранящемуся в контейнере объекту
	bool IsValid(Id id) const { return m_items.count(id); }

	// Получить объект по идентификатору
	const T& Get(Id id) const { return m_items.find(id)->second.m_data; }

	// Увеличить приоритет объекта на 1
	void Promote(Id id) {
		auto it = m_items.find(id);
		m_priority_set.erase(SetKey(
			it->first, it->second.m_priority));
		it->second.m_priority++;
		m_priority_set.insert(SetKey(
			it->first, it->second.m_priority));
	}

	// Получить объект с максимальным приоритетом и его приоритет
	pair<const T&, int> GetMax() const {
		auto it = m_items.find(m_priority_set.begin()->m_id);
		return pair<const T&, int>(
			it->second.m_data,
			it->second.m_priority);
	}

	// Аналогично GetMax, но удаляет элемент из контейнера
	pair<T, int> PopMax() {
		auto it = m_items.find(m_priority_set.begin()->m_id);
		pair<T, int> ret(
			move(it->second.m_data),
			it->second.m_priority);
		m_items.erase(it);
		m_priority_set.erase(
			m_priority_set.begin());
		return ret;
	}

private:

	Id m_next_id;

	struct MapItem {
		int m_priority;
		T	m_data;

		MapItem() : m_priority(0) {}

		MapItem(T&& data) : m_priority(0), m_data(move(data)) {}
	};

	map<Id, MapItem> m_items;

	struct SetKey {
		Id	m_id;
		int m_priority;

		SetKey(Id id, int priority) : m_id(id), m_priority(priority) {}

		friend bool operator < (const SetKey& lhs, const SetKey& rhs) {
			if (lhs.m_priority != rhs.m_priority)
				return lhs.m_priority > rhs.m_priority;
			return lhs.m_id > rhs.m_id;
		}
	};

	set<SetKey> m_priority_set;
};


class StringNonCopyable : public string {
public:
	using string::string;  // Позволяет использовать конструкторы строки
	StringNonCopyable(const StringNonCopyable&) = delete;
	StringNonCopyable(StringNonCopyable&&) = default;
	StringNonCopyable& operator=(const StringNonCopyable&) = delete;
	StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
	PriorityCollection<StringNonCopyable> strings;
	const auto white_id = strings.Add("white");
	const auto yellow_id = strings.Add("yellow");
	const auto red_id = strings.Add("red");

	strings.Promote(yellow_id);
	for (int i = 0; i < 2; ++i) {
		strings.Promote(red_id);
	}
	strings.Promote(yellow_id);
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "red");
		ASSERT_EQUAL(item.second, 2);
	}
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "yellow");
		ASSERT_EQUAL(item.second, 2);
	}
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "white");
		ASSERT_EQUAL(item.second, 0);
	}
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestNoCopy);
	return 0;
}
