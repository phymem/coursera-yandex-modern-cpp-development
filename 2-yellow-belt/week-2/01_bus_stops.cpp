#include <string>
#include <iostream>
#include <cassert>
#include <vector>
#include <map>

using namespace std;

enum class QueryType {
	NewBus,
	BusesForStop,
	StopsForBus,
	AllBuses
};

struct Query {
	QueryType type;
	string bus;
	string stop;
	vector<string> stops;
};

istream& operator >> (istream& is, Query& q) {
	std::string type;
	is >> type;

	if (type == "NEW_BUS") {
		q.type = QueryType::NewBus;

		is >> q.bus;

		unsigned int num;
		is >> num;
		q.stops.resize(num);
		for (auto& stop : q.stops) {
			is >> stop;
		}
	}
	else if (type == "BUSES_FOR_STOP") {
		q.type = QueryType::BusesForStop;
		is >> q.stop;
	}
	else if (type == "STOPS_FOR_BUS") {
		q.type = QueryType::StopsForBus;
		is >> q.bus;
	}
	else if (type == "ALL_BUSES") {
		q.type = QueryType::AllBuses;
	}
	else
		throw runtime_error("unexpected query type");

	return is;
}

struct BusesForStopResponse {
	vector<string> buses;
};

ostream& operator << (ostream& os, const BusesForStopResponse& r) {
	if (r.buses.empty()) {
		os << "No stop";
	}
	else {
		for (const auto& s : r.buses) {
			os << s << " ";
		}
	}
	return os;
}

struct StopsForBusResponse {
	vector<pair<string, vector<string>>> stops;
};

ostream& operator << (ostream& os, const StopsForBusResponse& r) {
	if (r.stops.empty()) {
		os << "No bus";
	}
	else {
		const char* delim = "";
		for (const auto& p : r.stops) {
			os << delim << "Stop " << p.first << ": ";
			for (const auto& s : p.second) {
				os << s << " ";
			}
			delim = "\n";
		}
	}
	return os;
}

struct AllBusesResponse {
	vector<pair<string, vector<string>>> buses;
};

ostream& operator << (ostream& os, const AllBusesResponse& r) {
	if (r.buses.empty()) {
		os << "No buses";
	}
	else {
		const char* delim = "";
		for (const auto& p : r.buses) {
			os << delim << "Bus " << p.first << ": ";
			for (const auto& s : p.second) {
				os << s << " ";
			}
			delim = "\n";
		}
	}
	return os;
}

class BusManager {
public:
	void AddBus(const string& bus, const vector<string>& vec) {
		buses_to_stops[bus] = vec;
		for (const auto& stop : vec) {
			stops_to_buses[stop].push_back(bus);
		}
	}

	BusesForStopResponse GetBusesForStop(const string& stop) const {
		BusesForStopResponse ret;
		if (stops_to_buses.count(stop)) {
			for (const auto& bus : stops_to_buses.at(stop)) {
				ret.buses.push_back(bus);
			}
		}
		return ret;
	}

	StopsForBusResponse GetStopsForBus(const string& bus) const {
		StopsForBusResponse ret;
		if (buses_to_stops.count(bus)) {
			for (const string& stop : buses_to_stops.at(bus)) {
				vector<string> buses;
				if (stops_to_buses.at(stop).size() == 1) {
					buses.push_back("no interchange");
				}
				else {
					for (const auto& other_bus : stops_to_buses.at(stop)) {
						if (bus != other_bus) {
							buses.push_back(other_bus);
						}
					}
				}
				ret.stops.push_back(make_pair(stop, buses));
			}
		}
		return ret;
	}

	AllBusesResponse GetAllBuses() const {
		AllBusesResponse ret;
		for (const auto& it : buses_to_stops) {
			ret.buses.push_back(it);
		}
		return ret;
	}

private:

	map<string, vector<string>> buses_to_stops, stops_to_buses;
};

int main() {
	int query_count;
	Query q;

	cin >> query_count;

	BusManager bm;
	for (int i = 0; i < query_count; ++i) {
		cin >> q;
		switch (q.type) {
		case QueryType::NewBus:
			bm.AddBus(q.bus, q.stops);
			break;
		case QueryType::BusesForStop:
			cout << bm.GetBusesForStop(q.stop) << endl;
			break;
		case QueryType::StopsForBus:
			cout << bm.GetStopsForBus(q.bus) << endl;
			break;
		case QueryType::AllBuses:
			cout << bm.GetAllBuses() << endl;
			break;
		}
	}

	return 0;
}
