#include "test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct Record {
	string id;
	string title;
	string user;
	int timestamp;
	int karma;
};

class Database {
public:

	bool Put(const Record& record) {
		auto [it, inserted]  = db.insert(make_pair(record.id, Data(record)));
		if (inserted) {
			Data& d = it->second;
			d.it_timestamp = index_timestamp.insert(
				make_pair(d.record.timestamp, &d.record));
			d.it_karma = index_karma.insert(
				make_pair(d.record.karma, &d.record));
			d.it_user = index_user.insert(
				make_pair(d.record.user, &d.record));
			return true;
		}
		return false;
	}

	const Record* GetById(const string& id) const {
		if (auto it = db.find(id); it != db.end())
			return &it->second.record;
		return nullptr;
	}

	bool Erase(const string& id) {
		if (auto it = db.find(id); it != db.end()) {
			index_timestamp.erase(it->second.it_timestamp);
			index_karma.erase(it->second.it_karma);
			index_user.erase(it->second.it_user);
			db.erase(it);
			return true;
		}
		return false;
	}

	template <typename Callback>
	void RangeByTimestamp(int low, int high, Callback callback) const {
		RangeBy(make_pair(
			index_timestamp.lower_bound(low),
			index_timestamp.upper_bound(high)),
			callback);
	}

	template <typename Callback>
	void RangeByKarma(int low, int high, Callback callback) const {
		RangeBy(make_pair(
			index_karma.lower_bound(low),
			index_karma.upper_bound(high)),
			callback);
	}

	template <typename Callback>
	void AllByUser(const string& user, Callback callback) const {
		RangeBy(index_user.equal_range(user), callback);
	}

private:

	using multimap_int_t = multimap<int, Record*>;
	using multimap_str_t = multimap<string, Record*>;

	struct Data {
		Data(const Record& rec) :
			record(rec) {}

		Record record;
		multimap_int_t::iterator it_timestamp;
		multimap_int_t::iterator it_karma;
		multimap_str_t::iterator it_user;
	};

	template <typename Range, typename Callback>
	void RangeBy(Range range, Callback callback) const {
		for (auto it = range.first; it != range.second; ++it) {
			if (!callback(*it->second))
				return;
		}
	}

private:

	multimap_int_t index_timestamp;
	multimap_int_t index_karma;
	multimap_str_t index_user;

	unordered_map<string, Data> db;
};

void TestRangeBoundaries() {
	const int good_karma = 1000;
	const int bad_karma = -10;

	Database db;
	db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
	db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

	int count = 0;
	db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
		++count;
		return true;
	});

	ASSERT_EQUAL(2, count);
}

void TestSameUser() {
	Database db;
	db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
	db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

	int count = 0;
	db.AllByUser("master", [&count](const Record&) {
		++count;
		return true;
	});

	ASSERT_EQUAL(2, count);
}

void TestReplacement() {
	const string final_body = "Feeling sad";

	Database db;
	db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
	db.Erase("id");
	db.Put({"id", final_body, "not-master", 1536107260, -10});

	auto record = db.GetById("id");
	ASSERT(record != nullptr);
	ASSERT_EQUAL(final_body, record->title);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestRangeBoundaries);
	RUN_TEST(tr, TestSameUser);
	RUN_TEST(tr, TestReplacement);
	return 0;
}
