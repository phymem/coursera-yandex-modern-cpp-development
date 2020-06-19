#pragma once

#include <cmath> // sin
#include <array>
#include <vector>
#include <string>
#include <memory> // unique_ptr
#include <algorithm>
#include <unordered_map>

#include "router.h"

class BusRoute;

const size_t MAX_NAME_LEN = 64;
const size_t MAX_DB_SIZE = 1000;
const size_t MAX_BUS_STOPS = 100;
const size_t MAX_DISTANCES = 100;

typedef Graph::DirectedWeightedGraph<double> RouteGraph_t;
typedef Graph::Router<double> BusRouter_t;

struct BusStop {
	size_t m_id;
	std::string_view m_name;
	double m_latitude;
	double m_longitude;

	std::vector<BusRoute*> m_buses;

	std::unordered_map<BusStop*, int> m_distances;

	BusStop(size_t f_id, std::string_view f_name) :
		m_id(f_id),
		m_name(f_name),
		m_latitude(0),
		m_longitude(0),
		m_buses_sorted(false)
	{
		m_buses.reserve(MAX_DB_SIZE);
		m_distances.reserve(MAX_DISTANCES);
	}

	bool m_buses_sorted;
	const std::vector<BusRoute*>& getBuses();
};

struct BusRoute {
	size_t m_id;
	std::string_view m_name;
	bool m_roundtrip;
	std::vector<BusStop*> m_stops;

	BusRoute(size_t f_id, std::string_view f_name) :
		m_id(f_id),
		m_name(f_name),
		m_roundtrip(false),
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
			auto route_distance = [](const BusStop* f_p1, const BusStop* f_p2) {
				const double radius = 6371000;
				return radius * acos(sin(f_p1->m_latitude) * sin(f_p2->m_latitude)
					+ cos(f_p1->m_latitude) * cos(f_p2->m_latitude) * cos(f_p1->m_longitude - f_p2->m_longitude));
			};
			for (unsigned int i = 1; i < m_stops.size(); ++i) {
				BusStop* prev = m_stops[i - 1];
				BusStop* next = m_stops[i];

				m_length.m_lenC += (!m_roundtrip + 1) * route_distance(prev, next);

				m_length.m_lenL += prev->m_distances[next];
				if (!m_roundtrip)
					m_length.m_lenL += next->m_distances[prev];
			}
		}
		return m_length;
	}

	unsigned int m_num_uniq_stops;
	unsigned int getNumUniqueStops() {
		if (!m_num_uniq_stops) {
			std::array<BusStop*, MAX_BUS_STOPS> tmp;
			std::copy(m_stops.begin(), m_stops.end(), tmp.begin());
			std::sort(tmp.begin(), tmp.begin() + m_stops.size());
			m_num_uniq_stops = std::unique(tmp.begin(), tmp.begin() + m_stops.size()) - tmp.begin();
		}
		return m_num_uniq_stops;
	}

};

inline const std::vector<BusRoute*>& BusStop::getBuses() {
	if (!m_buses_sorted) {
		m_buses_sorted = true;
		std::sort(m_buses.begin(), m_buses.end(),
			[](BusRoute* f_lhs, BusRoute* f_rhs)
			{ return f_lhs->m_name < f_rhs->m_name; });
		m_buses.erase(std::unique(m_buses.begin(), m_buses.end()), m_buses.end());
	}
	return m_buses;
}

struct RouteGraphEdge {
	size_t m_id;

	BusRoute* m_route;
	BusStop* m_stop_from;
	BusStop* m_stop_to;

	unsigned int m_wait_time;
	double m_span_time;
	unsigned int m_span_count;

	RouteGraphEdge(size_t f_id,
		BusRoute* f_route,
		BusStop* f_stop_from,
		BusStop* f_stop_to,
		unsigned int f_wait_time,
		double f_span_time,
		unsigned int f_span_count) :
		m_id(f_id),
		m_route(f_route),
		m_stop_from(f_stop_from),
		m_stop_to(f_stop_to),
		m_wait_time(f_wait_time),
		m_span_time(f_span_time),
		m_span_count(f_span_count) {}

	Graph::Edge<double> to_graph_edge() const {
		return Graph::Edge<double> {
			m_stop_from->m_id,
			m_stop_to->m_id,
			m_wait_time + m_span_time
		};
	}
};

class Database {
public:

	Database() :
		m_bus_wait_time(0),
		m_bus_velocity(0)
	{
		m_names.reserve(MAX_DB_SIZE * MAX_NAME_LEN);

		m_stops.reserve(MAX_DB_SIZE);
		m_stop_by_name.reserve(MAX_DB_SIZE);

		m_routes.reserve(MAX_DB_SIZE);
		m_route_by_name.reserve(MAX_DB_SIZE);

		m_route_graph_edges.reserve(MAX_DB_SIZE);
	}

	BusStop* getStop(std::string_view f_name) {
		if (auto it = m_stop_by_name.find(f_name); it != m_stop_by_name.end())
			return it->second;
		return nullptr;
	}

	BusStop* insertStop(std::string_view f_name) {
		if (BusStop* stop = getStop(f_name))
			return stop;
		std::string_view sv = make_string_view(f_name);
		m_stops.push_back(BusStop(m_stops.size(), sv));
		BusStop* stop = &m_stops[m_stops.size() - 1];
		m_stop_by_name[stop->m_name] = stop;
		return stop;

	}

	BusRoute* getRoute(std::string_view f_name) {
		if (auto it = m_route_by_name.find(f_name); it != m_route_by_name.end())
			return it->second;
		return nullptr;
	}

	BusRoute* insertRoute(std::string_view f_name) {
		std::string_view sv = make_string_view(f_name);
		m_routes.push_back(BusRoute(m_routes.size(), sv));
		BusRoute* route = &m_routes[m_routes.size() - 1];
		m_route_by_name[route->m_name] = route;
		return route;
	}

	void setRoutingSettings(int f_bus_wait_time, double f_bus_velocity) {
		m_bus_wait_time = f_bus_wait_time;
		m_bus_velocity = f_bus_velocity;
	}

	std::optional<BusRouter_t::RouteInfo> buildRoute(BusStop* f_stop_from, BusStop* f_stop_to) {
		if (!f_stop_from || !f_stop_to)
			return {};

		if (!m_route_graph.get()) {
			PERF_SENSOR("routeGraph");

			m_route_graph = std::make_unique<RouteGraph_t>(m_stops.size());
			for (BusRoute& bus : m_routes) {
				for (auto it_stop = bus.m_stops.begin(); (it_stop + 1) != bus.m_stops.end(); ++it_stop) {
					add_graph_route(&bus, *it_stop, it_stop + 1, bus.m_stops.end());
				}
				if (!bus.m_roundtrip) {
					for (auto it_stop = bus.m_stops.rbegin(); (it_stop + 1) != bus.m_stops.rend(); ++it_stop) {
						add_graph_route(&bus, *it_stop, it_stop + 1, bus.m_stops.rend());
					}
				}
			}

			m_bus_router = std::make_unique<BusRouter_t>(*m_route_graph.get());
		}

		PERF_SENSOR("buildRoute");
		return m_bus_router->BuildRoute(f_stop_from->m_id, f_stop_to->m_id);
	}

	const RouteGraphEdge* getRouteEdge(BusRouter_t::RouteId f_route_id, unsigned int f_edge_ndx) {
		return &m_route_graph_edges[m_bus_router->GetRouteEdge(f_route_id, f_edge_ndx)];
	}

private:

	std::string_view make_string_view(std::string_view f_sv) {
		size_t pos = m_names.length();
		m_names += f_sv;
		return std::string_view(m_names.c_str() + pos, f_sv.length());
	}

	template <typename iterator>
	void add_graph_route(BusRoute* f_route, BusStop* f_stop_from, iterator f_first, iterator f_last) {
		BusStop* stop_prev = f_stop_from;
		unsigned int spans = 0;
		unsigned int dist = 0;

		for (iterator i = f_first; i != f_last; ++i) {
			BusStop* stop_to = *i;
			dist += stop_prev->m_distances[stop_to];
			stop_prev = stop_to;

			if (f_stop_from == stop_to)
				continue;

			m_route_graph_edges.push_back(
				RouteGraphEdge(m_route_graph_edges.size(),
					f_route, f_stop_from, stop_to, m_bus_wait_time,
					(double)dist / m_bus_velocity, ++spans));

			size_t id = m_route_graph->AddEdge(
				m_route_graph_edges.back().to_graph_edge());
			assert(id == m_route_graph_edges.back().m_id);
		}
	}

private:

	std::string m_names;

	unsigned int m_bus_wait_time;
	double m_bus_velocity;

	std::unique_ptr<BusRouter_t> m_bus_router;
	std::unique_ptr<RouteGraph_t> m_route_graph;
	std::vector<RouteGraphEdge> m_route_graph_edges;

	std::vector<BusStop> m_stops;
	std::unordered_map<std::string_view, BusStop*> m_stop_by_name;

	std::vector<BusRoute> m_routes;
	std::unordered_map<std::string_view, BusRoute*> m_route_by_name;
};
