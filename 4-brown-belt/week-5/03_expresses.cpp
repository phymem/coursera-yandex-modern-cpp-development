#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>
#include <set>

using namespace std;

class RouteManager {
public:
	void AddRoute(int start, int finish) {
		reachable_lists_[start].insert(finish);
		reachable_lists_[finish].insert(start);
	}
	int FindNearestFinish(int start, int finish) const {
		int result = abs(start - finish);
		auto it = reachable_lists_.find(start);
		if (it == reachable_lists_.end()) {
			return result;
		}
		const set<int>& reachable_stations = it->second;
		if (!reachable_stations.empty()) {
			auto lb = reachable_stations.lower_bound(finish);
			if (lb != reachable_stations.end()) {
				result = min(result, abs(*lb - finish));
			}
			if (result && lb != reachable_stations.begin()) {
				result = min(result, abs(*prev(lb) - finish));
			}
		}
		return result;
	}
private:
	unordered_map<int, set<int>> reachable_lists_;
};


int main() {
	RouteManager routes;

	int query_count;
	cin >> query_count;

	for (int query_id = 0; query_id < query_count; ++query_id) {
		string query_type;
		cin >> query_type;
		int start, finish;
		cin >> start >> finish;
		if (query_type == "ADD") {
			routes.AddRoute(start, finish);
		} else if (query_type == "GO") {
			cout << routes.FindNearestFinish(start, finish) << "\n";
		}
	}

	return 0;
}
