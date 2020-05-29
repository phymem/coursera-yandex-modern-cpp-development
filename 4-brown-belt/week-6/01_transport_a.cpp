#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <cctype>
#include <vector>
#include <string>
#include <cmath>
#include <array>

//#include "profile.h"

using namespace std;

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

	BusStop(string_view f_name) :
		m_name(f_name),
		m_latitude(0),
		m_longitude(0) {}
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
		m_route_length(0),
		m_num_uniq_stops(0)
		{ m_stops.reserve(MAX_BUS_STOPS); }

	double m_route_length;
	double getRouteLength() {
		if (!m_route_length) {
			for (unsigned int i = 1; i < m_stops.size(); ++i) {
				m_route_length += route_distance(m_stops[i - 1], m_stops[i]);
			}
			if (m_circular)
				m_route_length *= 2;
		}
		return m_route_length;
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
	BusStop* stop = f_db->getStop(buf.read([](char f_ch)
		{ return f_ch != ':'; }).to_string_view());

	char* end = nullptr;
	const double to_rad = 3.1415926535 / 180.0;

	stop->m_latitude = to_rad * strtod(buf.read([](char f_ch)
		{ return f_ch != ','; }).to_c_str(), &end);
	if (!end || *end)
		throw invalid_argument("readStop: invalid strtod conversion");

	stop->m_longitude = to_rad * strtod(buf.read([](char f_ch)
		{ return f_ch != '\n' && f_ch != '\r'; }).to_c_str(), &end);
	if (!end || *end)
		throw invalid_argument("readStop: invalid strtod conversion");
}

void readBus(Database* f_db) {
	PERF_SENSOR("readBus");

	ReadBuf buf;
	BusRoute* route = f_db->insertRoute(
		buf.read([](char f_ch) { return f_ch != ':'; }).to_string_view());

	char ch;
	do {
		buf.read([](char f_ch) {
			return f_ch != '-' && f_ch != '>' && f_ch != '\n' && f_ch != '\r'; }, &ch);

		if (ch == '-')
			route->m_circular = true;

		route->m_stops.push_back(f_db->getStop(buf.to_string_view()));

	} while (ch != '\n' && ch != '\r');
}

void reportBus(Database* f_db) {
	PERF_SENSOR("reportBus");

	ReadBuf buf;
	BusRoute* route = f_db->getRoute(buf.read([](char f_ch)
		{ return f_ch != '\n' && f_ch != '\r'; }).to_string_view());
	if (route == nullptr) {
		fprintf(stdout, "Bus %s: not found\n", buf.to_c_str());
		return;
	}

	int num_stops = route->m_stops.size();
	if (route->m_circular)
		num_stops = 2 * num_stops - 1;

	fprintf(stdout, "Bus %s: %d stops on route, %d unique stops, %.6f route length\n",
		buf.to_c_str(), num_stops, route->getNumUniqueStops(), route->getRouteLength());
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
		else
			throw invalid_argument("invalid report");
	}
}

int main() {

	execute();

	PERF_DUMP();

	return 0;
}
