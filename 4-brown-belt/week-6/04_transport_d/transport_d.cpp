#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cctype>
#include <vector>
#include <string>
#include <cmath>
#include <array>

#include "json.h"

//#include "profile.h"

using namespace std;

class BusStop;
class BusRoute;

#ifdef __PROFILE_H__
	vector<ProfileData> ProfileData::m_data;
	#define PERF_SENSOR(name) ProfileSensor ps##__LINE__(name)
	#define PERF_DUMP() ProfileData::dump()
#else
	#define PERF_SENSOR(name) (void)name
	#define PERF_DUMP()
#endif

const size_t MAX_NAME_LEN = 64;
const size_t MAX_DB_SIZE = 2000;
const size_t MAX_BUS_STOPS = 100;
const size_t MAX_DISTANCES = 100;

struct BusStop {
	string_view m_name;
	double m_latitude;
	double m_longitude;

	vector<BusRoute*> m_buses;

	unordered_map<BusStop*, int> m_distances;

	BusStop(string_view f_name) :
		m_name(f_name),
		m_latitude(0),
		m_longitude(0),
		m_buses_sorted(false)
	{
		m_buses.reserve(MAX_DB_SIZE);
		m_distances.reserve(MAX_DISTANCES);
	}

	bool m_buses_sorted;
	const vector<BusRoute*>& getBuses();
};

inline double route_distance(const BusStop* f_p1, const BusStop* f_p2) {
	const double radius = 6371000;
	return radius * acos(sin(f_p1->m_latitude) * sin(f_p2->m_latitude)
		+ cos(f_p1->m_latitude) * cos(f_p2->m_latitude) * cos(f_p1->m_longitude - f_p2->m_longitude));
}

struct BusRoute {
	string_view m_name;
	bool m_circle;
	vector<BusStop*> m_stops;

	BusRoute(string_view f_name) :
		m_name(f_name),
		m_circle(false),
		m_num_uniq_stops(0)
		{ m_stops.reserve(MAX_BUS_STOPS); }

	struct RouteLength {
		int m_lenL;
		double m_lenC;

		RouteLength() : m_lenL(0), m_lenC(0) {}
	};

	RouteLength m_length;

	const RouteLength& getRouteLength() {
		if (!m_length.m_lenC) {
			for (unsigned int i = 1; i < m_stops.size(); ++i) {
				BusStop* prev = m_stops[i - 1];
				BusStop* next = m_stops[i];

				m_length.m_lenC += (m_circle + 1) * route_distance(prev, next);

				m_length.m_lenL += prev->m_distances[next];
				if (m_circle)
					m_length.m_lenL += next->m_distances[prev];
			}
		}
		return m_length;
	}

	unsigned int m_num_uniq_stops;
	unsigned int getNumUniqueStops() {
		if (!m_num_uniq_stops) {
			array<BusStop*, MAX_BUS_STOPS> tmp;
			copy(m_stops.begin(), m_stops.end(), tmp.begin());
			sort(tmp.begin(), tmp.begin() + m_stops.size());
			m_num_uniq_stops = unique(tmp.begin(), tmp.begin() + m_stops.size()) - tmp.begin();
		}
		return m_num_uniq_stops;
	}

};

inline const vector<BusRoute*>& BusStop::getBuses() {
	if (!m_buses_sorted) {
		m_buses_sorted = true;
		sort(m_buses.begin(), m_buses.end(),
			[](BusRoute* f_lhs, BusRoute* f_rhs)
			{ return f_lhs->m_name < f_rhs->m_name; });
		m_buses.erase(unique(m_buses.begin(), m_buses.end()), m_buses.end());
	}
	return m_buses;
}

class Database {
public:

	Database() {
		m_names.reserve(MAX_DB_SIZE * MAX_NAME_LEN);
		m_stops.reserve(MAX_DB_SIZE);
		m_routes.reserve(MAX_DB_SIZE);
	}

	BusStop* getStop(string_view f_name) {
		auto it = m_stops.find(f_name);
		if (it != m_stops.end())
			return &it->second;
		return nullptr;
	}

	BusStop* insertStop(string_view f_name) {
		if (BusStop* stop = getStop(f_name))
			return stop;
		string_view sv = make_string_view(f_name);
		auto ret = m_stops.insert(make_pair(sv, BusStop(sv)));
		return &ret.first->second;

	}

	BusRoute* getRoute(string_view f_name) {
		auto it = m_routes.find(f_name);
		if (it != m_routes.end())
			return &it->second;
		return nullptr;
	}

	BusRoute* insertRoute(string_view f_name) {
		string_view sv = make_string_view(f_name);
		auto ret = m_routes.insert(make_pair(sv, BusRoute(sv)));
		return &ret.first->second;
	}

private:

	string_view make_string_view(string_view f_sv) {
		size_t pos = m_names.length();
		m_names += f_sv;
		return string_view(m_names.c_str() + pos, f_sv.length());
	}

private:

	string m_names;

	unordered_map<string_view, BusStop> m_stops;
	unordered_map<string_view, BusRoute> m_routes;
};

void readStop(const map<string, Json::Node>& f_req, Database* f_db) {
	PERF_SENSOR("readStop");

	auto it = f_req.find("name");
	if (it == f_req.end()) {
		throw invalid_argument("readStop: name");
	}
	BusStop* stop = f_db->insertStop(it->second.AsString());

	const double to_rad = 3.1415926535 / 180.0;

	it = f_req.find("longitude");
	if (it == f_req.end()) {
		throw invalid_argument("readStop: longitude");
	}
	stop->m_longitude = to_rad * it->second.AsDouble();

	it = f_req.find("latitude");
	if (it == f_req.end()) {
		throw invalid_argument("readStop: latitude");
	}
	stop->m_latitude = to_rad * it->second.AsDouble();

	it = f_req.find("road_distances");
	if (it != f_req.end()) {
		for (auto p : it->second.AsMap()) {
			BusStop* to_stop = f_db->insertStop(p.first);
			int dist = p.second.AsInt();

			stop->m_distances[to_stop] = dist;
			if (auto i = to_stop->m_distances.find(stop); i == to_stop->m_distances.end())
				to_stop->m_distances[stop] = dist;
		}
	}
}

void readBus(const map<string, Json::Node>& f_req, Database* f_db) {
	PERF_SENSOR("readBus");

	auto it = f_req.find("name");
	if (it == f_req.end()) {
		throw invalid_argument("readBus: name");
	}
	BusRoute* route = f_db->insertRoute(it->second.AsString());

	it = f_req.find("is_roundtrip");
	if (it == f_req.end()) {
		throw invalid_argument("readBus: is_roundtrip");
	}
	route->m_circle = it->second.AsString() == "false";

	it = f_req.find("stops");
	if (it == f_req.end()) {
		throw invalid_argument("readBus: stops");
	}
	for (const auto& a : it->second.AsArray()) {
		BusStop* stop = f_db->insertStop(a.AsString());
		stop->m_buses.push_back(route);
		route->m_stops.push_back(stop);
	}
}

void reportBus(const char* f_prefix, const map<string, Json::Node>& f_req, Database* f_db) {
	PERF_SENSOR("reportBus");

	auto it = f_req.find("name");
	if (it == f_req.end()) {
		throw invalid_argument("reportBus: name");
	}
	BusRoute* route = f_db->getRoute(it->second.AsString());
	if (route == nullptr) {
		cout << f_prefix << "\"error_message\": \"not found\"\n";
		return;
	}

	const BusRoute::RouteLength& len = route->getRouteLength();

	int num_stops = route->m_stops.size();
	if (route->m_circle)
		num_stops = 2 * num_stops - 1;

	cout << f_prefix << "\"route_length\": " << len.m_lenL << ",\n"
		<< f_prefix << "\"curvature\": " << (double)len.m_lenL / len.m_lenC << ",\n"
		<< f_prefix << "\"stop_count\": " << num_stops << ",\n"
		<< f_prefix << "\"unique_stop_count\": " << route->getNumUniqueStops() << "\n";
}

void reportStop(const char* f_prefix, const map<string, Json::Node>& f_req, Database* f_db) {
	PERF_SENSOR("reportStop");

	auto it = f_req.find("name");
	if (it == f_req.end()) {
		throw invalid_argument("reportStop: name");
	}
	BusStop* stop = f_db->getStop(it->second.AsString());
	if (stop == nullptr) {
		cout << f_prefix << "\"error_message\": \"not found\"\n";
		return;
	}

	if (stop->m_buses.empty()) {
		cout << f_prefix << "\"buses\": []\n";
	}
	else {
		cout << f_prefix << "\"buses\": [\n";
		const auto& buses = stop->getBuses();
		for (unsigned int i = 0; i < buses.size(); ++i) {
			cout << f_prefix << "  \"" << string(buses[i]->m_name)
				<< ((i == buses.size() - 1) ? "\"\n" : "\",\n");
		}
		cout << f_prefix << "]\n";
	}
}

void parseBaseRequests(const Json::Node& f_node, Database* f_db) {
	for (const Json::Node& node : f_node.AsArray()) {
		const map<string, Json::Node>& req = node.AsMap();

		auto it = req.find("type");
		if (it == req.end()) {
			throw invalid_argument("parseBaseRequests: type");
		}
		const string& type = it->second.AsString();

		if (type == "Stop")
			readStop(req, f_db);
		else if (type == "Bus")
			readBus(req, f_db);
		else
			throw invalid_argument("parseBaseRequests: read");
	}
}

void parseStatRequests(const Json::Node& f_node, Database* f_db) {
	cout << "[\n";
	for (unsigned int i = 0; i < f_node.AsArray().size(); ++i) {
		const map<string, Json::Node>& req = f_node.AsArray()[i].AsMap();

		auto it = req.find("id");
		if (it == req.end()) {
			throw invalid_argument("parseStatRequests: id");
		}
		cout << "  {\n    \"request_id\": " << it->second.AsString() << ",\n";

		it = req.find("type");
		if (it == req.end()) {
			throw invalid_argument("parseBaseRequests: type");
		}
		const string& type = it->second.AsString();

		if (type == "Stop")
			reportStop("    ", req, f_db);
		else if (type == "Bus")
			reportBus("    ", req, f_db);
		else
			throw invalid_argument("parseStatRequests: report");

		if (i == f_node.AsArray().size() - 1)
			cout << "  }\n";
		else
			cout << "  },\n";
	}
	cout << "]\n";
}

void execute() {

	PERF_SENSOR("execute");

	Json::Document doc = Json::Load(cin);

	Database db;

	const auto& req_map = doc.GetRoot().AsMap();

	if (auto it = req_map.find("base_requests"); it != req_map.end()) {
		parseBaseRequests(it->second, &db);
	}

	if (auto it = req_map.find("stat_requests"); it != req_map.end()) {
		parseStatRequests(it->second, &db);
	}
}

int main() {

	execute();

	PERF_DUMP();

	return 0;
}
