#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

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
		if (it == end(reachable_lists_))
			return result;

		const set<int>& reachable_stations = it->second;
		const auto lb = reachable_stations.lower_bound(finish);

		int closest = start;
		if (lb != reachable_stations.end())
			closest = *lb;
		if (lb != reachable_stations.begin()) {
			int station = *prev(lb);
			if (abs(station - finish) < abs(closest - finish))
				closest = station;
		}

		return min(result, abs(finish - closest));
	}
private:
	map<int, set<int>> reachable_lists_;
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
