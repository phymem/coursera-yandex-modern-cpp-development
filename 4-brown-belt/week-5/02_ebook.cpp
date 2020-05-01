#include <iomanip>
#include <iostream>
#include <vector>
#include <utility>
#include <map>

using namespace std;

class ReadingManager {
public:
	ReadingManager() :
		user_page_counts_(MAX_USER_COUNT_ + 1, 0),
		num_users_(0) {}


	void Read(int user_id, int page_count) {
		int prev_page_count = user_page_counts_[user_id];
		if (!prev_page_count) {
			num_users_++;
		}
		else {
			auto it = page_to_num_users_.find(prev_page_count);
			if (it->second == 1)
				page_to_num_users_.erase(it);
			else
				it->second--;
		}
		user_page_counts_[user_id] = page_count;
		page_to_num_users_[page_count]++;
	}

	double Cheer(int user_id) const {
		int page_count = user_page_counts_[user_id];
		if (!page_count)
			return 0;
		if (num_users_ == 1)
			return 1;
		int num_users_with_less_pages = 0;
		for (auto& p : page_to_num_users_) {
			if (p.first >= page_count)
				break;
			num_users_with_less_pages += p.second;
		}
		// По умолчанию деление целочисленное, поэтому
		// нужно привести числитель к типу double.
		// Простой способ сделать это — умножить его на 1.0.
		return (num_users_with_less_pages) * 1.0 / (num_users_ - 1);
	}

private:
	// Статическое поле не принадлежит какому-то конкретному
	// объекту класса. По сути это глобальная переменная,
	// в данном случае константная.
	// Будь она публичной, к ней можно было бы обратиться снаружи
	// следующим образом: ReadingManager::MAX_USER_COUNT.
	static const int MAX_USER_COUNT_ = 100'000;

	vector<int> user_page_counts_;
	map<int, int> page_to_num_users_;
	unsigned int num_users_;
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
