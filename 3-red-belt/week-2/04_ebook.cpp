#include <iomanip>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

class ReadingManager {
public:
	ReadingManager() {
		pages_to_count_.resize(1000 /*max pages*/);
	}

	void Read(int user_id, int page_count) {
		auto it = users_to_pages_.find(user_id);
		if (it != end(users_to_pages_)) {
			--pages_to_count_[it->second];
		}
		pages_to_count_[page_count]++;
		users_to_pages_[user_id] = page_count;
	}

	double Cheer(int user_id) const {
		auto it = users_to_pages_.find(user_id);
		if (it == end(users_to_pages_))
			return 0;
		if (users_to_pages_.size() == 1)
			return 1;

		int ret = 0;
		for (unsigned int i = 0; i < it->second; ++i) {
			ret += pages_to_count_[i];
		}
		return (double)ret / (double)(users_to_pages_.size() - 1);
	}

private:

	vector<int> pages_to_count_;
	map<int, int> users_to_pages_;
};


int main() {
	// Для ускорения чтения данных отключается синхронизация
	// cin и cout с stdio,
	// а также выполняется отвязка cin от cout
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	ReadingManager manager;

	int query_count;
	cin >> query_count;

	for (int query_id = 0; query_id < query_count; ++query_id) {
		string query_type;
		cin >> query_type;
		int user_id;
		cin >> user_id;

		if (query_type == "READ") {
			int page_count;
			cin >> page_count;
			manager.Read(user_id, page_count);
		} else if (query_type == "CHEER") {
			cout << setprecision(6) << manager.Cheer(user_id) << "\n";
		}
	}

	return 0;
}
