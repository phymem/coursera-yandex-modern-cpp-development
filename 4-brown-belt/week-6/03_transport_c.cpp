#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <cctype>
#include <vector>
#include <string>
#include <cmath>
#include <array>

#include "profile.h"

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

struct ReadBuf {
	char m_buf[128];
	size_t m_len;

	ReadBuf() { clear(); }

	void clear() {
		m_buf[m_len = 0] = '\0';
	}

	template <typename predicate>
	ReadBuf& read(predicate f_pred, char* f_ch = nullptr) {
		clear();
		char ch;
		while (isspace(ch = getchar())) {}
		if (!f_pred(ch)) {
			if (f_ch) *f_ch = ch;
			return *this;
		}
		m_buf[m_len++] = ch;
		while (f_pred(ch = getchar())) {
			m_buf[m_len++] = ch;
			if (m_len >= sizeof(m_buf))
				throw invalid_argument("read_buf: buffer overflow");
		}
		m_buf[m_len] = '\0';
		// cut trailing spaces
		while (m_len && isspace(m_buf[m_len - 1])) {
			m_buf[--m_len] = '\0';
		}
		if (f_ch) *f_ch = ch;
		return *this;
	}

	const char* to_c_str() const  { return m_buf; }

	string_view to_string_view() const { return string_view(m_buf, m_len); }
};

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
	bool m_circular;
	vector<BusStop*> m_stops;

	BusRoute(string_view f_name) :
		m_name(f_name),
		m_circular(false),
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

				m_length.m_lenC += (m_circular + 1) * route_distance(prev, next);

				m_length.m_lenL += prev->m_distances[next];
				if (m_circular)
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

void readStop(Database* f_db) {
	PERF_SENSOR("readStop");

	ReadBuf buf;
	BusStop* stop = f_db->insertStop(buf.read([](char f_ch)
		{ return f_ch != ':'; }).to_string_view());

	char* end = nullptr;
	const double to_rad = 3.1415926535 / 180.0;

	stop->m_latitude = to_rad * strtod(buf.read([](char f_ch)
		{ return f_ch != ','; }).to_c_str(), &end);
	if (!end || *end)
		throw invalid_argument("readStop: invalid strtod conversion");

	char ch;
	stop->m_longitude = to_rad * strtod(buf.read([](char f_ch)
		{ return f_ch != '\n' && f_ch != '\r' && f_ch != ','; }, &ch).to_c_str(), &end);
	if (!end || *end)
		throw invalid_argument("readStop: invalid strtod conversion");

	while (ch != '\n' && ch != '\r') {
		int dist = atoi(buf.read([](char f_ch) { return f_ch != 'm'; }).to_c_str());

		// skip 'to '
		buf.read([](char f_ch) { return f_ch != 'o'; });

		BusStop* to_stop = f_db->insertStop(buf.read([](char f_ch)
			{ return f_ch != '\n' && f_ch != '\r' && f_ch != ','; }, &ch).to_string_view());

		stop->m_distances[to_stop] = dist;

		if (auto it = to_stop->m_distances.find(stop); it == to_stop->m_distances.end())
			to_stop->m_distances[stop] = dist;
	}
}

void readBus(Database* f_db) {
	PERF_SENSOR("readBus");

	ReadBuf buf;
	BusRoute* route = f_db->insertRoute(buf.read(
		[](char f_ch) { return f_ch != ':'; }).to_string_view());

	char ch;
	do {
		buf.read([](char f_ch) {
			return f_ch != '-' && f_ch != '>' && f_ch != '\n' && f_ch != '\r'; }, &ch);

		if (ch == '-')
			route->m_circular = true;

		BusStop* stop = f_db->insertStop(buf.to_string_view());
		stop->m_buses.push_back(route);
		route->m_stops.push_back(stop);

	} while (ch != '\n' && ch != '\r');
}

void reportBus(Database* f_db) {
	PERF_SENSOR("reportBus");

	ReadBuf buf;
	BusRoute* route = f_db->getRoute(buf.read(
		[](char f_ch) { return f_ch != '\n' && f_ch != '\r'; }).to_string_view());
	if (route == nullptr) {
		fprintf(stdout, "Bus %s: not found\n", buf.to_c_str());
		return;
	}

	int num_stops = route->m_stops.size();
	if (route->m_circular)
		num_stops = 2 * num_stops - 1;

	const BusRoute::RouteLength& len = route->getRouteLength();

	fprintf(stdout, "Bus %s: %d stops on route, %d unique stops, %d route length, %.6f curvature\n",
		buf.to_c_str(), num_stops, route->getNumUniqueStops(), len.m_lenL, (double)len.m_lenL / len.m_lenC);
}

void reportStop(Database* f_db) {
	PERF_SENSOR("reportStop");

	ReadBuf buf;
	buf.read([](char f_ch) { return f_ch != '\n' && f_ch != '\r'; });
	fprintf(stdout, "Stop %s:", buf.to_c_str());

	BusStop* stop = f_db->getStop(buf.to_string_view());
	if (!stop) {
		fprintf(stdout, " not found\n");
		return;
	}

	if (stop->m_buses.empty()) {
		fprintf(stdout, " no buses");
	}
	else {
		fprintf(stdout, " buses");
		for (BusRoute* route : stop->getBuses()) {
			fprintf(stdout, " %s", string(route->m_name).c_str());
		}
	}
	fprintf(stdout, "\n");
}

void execute() {

	PERF_SENSOR("execute");

	Database db;

	ReadBuf buf;
	int num = atoi(buf.read([](char f_ch)
		{ return f_ch != '\n' && f_ch != '\r'; }).to_c_str());
	while (num--) {

		buf.read([](char f_ch){ return isalpha(f_ch); });

		if (!strcmp("Stop", buf.to_c_str()))
			readStop(&db);
		else if (!strcmp("Bus", buf.to_c_str()))
			readBus(&db);
		else
			throw invalid_argument("invalid input");
	}

	num = atoi(buf.read([](char f_ch)
		{ return f_ch != '\n' && f_ch != '\r'; }).to_c_str());
	while (num--) {

		buf.read([](char f_ch) { return isalpha(f_ch); });

		if (!strcmp("Bus", buf.to_c_str()))
			reportBus(&db);
		else if (!strcmp("Stop", buf.to_c_str()))
			reportStop(&db);
		else
			throw invalid_argument("invalid report");
	}
}

int main() {

	execute();

	PERF_DUMP();

	return 0;
}
