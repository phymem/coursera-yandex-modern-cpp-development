//#include "profile.h"

#ifdef __PROFILE_H__
	std::vector<ProfileData> ProfileData::m_data;
	#define PERF_SENSOR(name) ProfileSensor ps##__LINE__(name)
	#define PERF_DUMP() ProfileData::dump()
#else
	#define PERF_SENSOR(name) (void)name
	#define PERF_DUMP()
#endif

#include "database.h"
#include "readbuf.h"
#include "json.h"

using namespace std;

class JsonCallbacks : public JsonParser::Callbacks {
public:

	JsonCallbacks() {}

	void setRoutingSettings(int f_bus_wait_time, double f_bus_velocity) override {
		m_db.setRoutingSettings(f_bus_wait_time, f_bus_velocity * 1000 / 60 /* meters/mins */);
	}

	void addBusStop(string_view f_name,
		double f_longitude, double f_latitude,
		const JsonParser::bus_stop_vec_t& f_distances) override {
		PERF_SENSOR("addBusStop");

		BusStop* stop = m_db.insertStop(f_name);

		const double to_rad = 3.1415926535 / 180.0;
		stop->m_longitude = to_rad * f_longitude;
		stop->m_latitude = to_rad * f_latitude;

		for (const auto& d : f_distances) {
			BusStop* to_stop = m_db.insertStop(d.m_name);
			stop->m_distances[to_stop] = d.m_distance;
			if (auto i = to_stop->m_distances.find(stop); i == to_stop->m_distances.end())
				to_stop->m_distances[stop] = d.m_distance;
		}
	}

	void addBusRoute(string_view f_name, bool f_roundtrip,
		const JsonParser::bus_stop_vec_t& f_stops) override {
		PERF_SENSOR("addBusRoute");

		BusRoute* route = m_db.insertRoute(f_name);
		route->m_roundtrip = f_roundtrip;
		for (const auto& d : f_stops) {
			BusStop* stop = m_db.insertStop(d.m_name);
			stop->m_buses.push_back(route);
			route->m_stops.push_back(stop);
		}
	}

	void reportBus(string_view f_name) override {
		PERF_SENSOR("reportBus");

		BusRoute* route = m_db.getRoute(f_name);
		if (route == nullptr) {
			fprintf(stdout, "\"error_message\": \"not found\"\n");
			return;
		}

		const BusRoute::RouteLength& len = route->getRouteLength();
		int num_stops = route->m_stops.size();
		if (!route->m_roundtrip)
			num_stops = 2 * num_stops - 1;

		fprintf(stdout,
			"    \"route_length\": %d,\n"
			"    \"curvature\": %.6g,\n"
			"    \"stop_count\": %d,\n"
			"    \"unique_stop_count\": %d\n",
			len.m_lenL,
			(double)len.m_lenL / len.m_lenC,
			num_stops,
			route->getNumUniqueStops());
	}

	void reportStop(string_view f_name) override {
		PERF_SENSOR("reportStop");

		BusStop* stop = m_db.getStop(f_name);
		if (stop == nullptr) {
			fprintf(stdout, "    \"error_message\": \"not found\"\n");
			return;
		}

		if (stop->m_buses.empty()) {
			fprintf(stdout, "    \"buses\": []\n");
		}
		else {
			fprintf(stdout, "    \"buses\": [\n");
			const auto& buses = stop->getBuses();
			for (unsigned int i = 0; i < buses.size(); ++i) {
				fprintf(stdout, "      \"%s\"%s",
					string(buses[i]->m_name).c_str(),
					(i == buses.size() - 1) ? "\n" : ",\n");
			}
			fprintf(stdout, "    ]\n");
		}
	}

	void reportRoute(string_view f_from, string_view f_to) override {
		PERF_SENSOR("reportRoute");

		auto route = m_db.buildRoute(m_db.getStop(f_from), m_db.getStop(f_to));
		if (!route) {
			fprintf(stdout, "    \"error_message\": \"not found\"\n");
			return;
		}

		fprintf(stdout,
			"    \"total_time\": %.6g,\n"
			"    \"items\": [\n",
			route->weight);
		for (unsigned int i = 0; i < route->edge_count; ++i) {
			const RouteGraphEdge* edge = m_db.getRouteEdge(route->id, i);
			fprintf(stdout,
				"        {\n"
				"            \"type\": \"Wait\",\n"
				"            \"stop_name\": \"%s\",\n"
				"            \"time\": %d\n"
				"        },\n"
				"        {\n"
				"            \"type\": \"Bus\",\n"
				"            \"bus\": \"%s\",\n"
				"            \"time\": %.6g,\n"
				"            \"span_count\": %d\n"
				"        }%s",
				string(edge->m_stop_from->m_name).c_str(),
				edge->m_wait_time,
				string(edge->m_route->m_name).c_str(),
				edge->m_span_time,
				edge->m_span_count,
				(i == route->edge_count - 1) ? "\n" : ",\n");
		}
		fprintf(stdout, "    ]\n");
	}

private:

	Database m_db;
};


void execute() {
	PERF_SENSOR("execute");

	JsonCallbacks json_callbacks;
	JsonParser fast_json(&json_callbacks);

	fast_json.parse();
}

int main() {

	execute();

	PERF_DUMP();

	return 0;
}
