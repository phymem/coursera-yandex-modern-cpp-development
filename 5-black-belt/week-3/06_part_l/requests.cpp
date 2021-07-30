#include "requests.h"
#include "descriptions.h"
#include "transport_router.h"
#include "transport_map.h"

#include <variant>
#include <vector>

using namespace std;

namespace {

template <typename T>
inline ostream& operator << (ostream& os, const pair<const char*, T>& p) {
	return os << '"' << p.first << "\": " << p.second;
}
inline ostream& operator << (ostream& os, const pair<const char*, const char*>& p ) {
	return os << '"' << p.first << "\": \"" << p.second << '"';
}

void ProcessBus(ostream& os, const Descriptions::Bus* bus) {
	if (!bus) {
		os << make_pair("error_message", "not found");
		return;
	}
	os << make_pair("stop_count", bus->stops.size()) << ", "
	<< make_pair("unique_stop_count", bus->unique_stop_count) << ", "
	<< make_pair("route_length",bus->road_route_length) << ", "
	<< make_pair("curvature", bus->road_route_length / bus->geo_route_distance);
}

void ProcessStop(ostream& os, const Descriptions::Stop* stop) {
	if (!stop) {
		os << make_pair("error_message", "not found");
		return;
	}
	os << "\"buses\": [";
	const char* delim = "";
	for (const auto& [_, bus] : stop->buses) {
		os << delim << '"' << bus->name << '"';
		delim = ", ";
	}
	os << ']';
}

void ProcessRoute(ostream& os, const TransportRouter& router, const Json::Dict& dict) {
	const auto route = router.FindRoute(
		dict.at("from").AsString(),
		dict.at("to").AsString()
	);
	if (!route) {
		os << make_pair("error_message", "not found");
		return;
	}
	os << make_pair("total_time", route->total_time) << ", \"items\": [";
	const char* delim = "";
	for (const auto& item : route->items) {
		os << delim << '{';
		delim = ", ";
		struct processor {
			ostream& os;
			void operator() (const TransportRouter::RouteInfo::BusItem& bus) {
				os << make_pair("type", "Bus") << ", "
				<< make_pair("bus", bus.bus->name.c_str()) << ", "
				<< make_pair("time", bus.time) << ", "
				<< make_pair("span_count", bus.span_count);
			}
			void operator() (const TransportRouter::RouteInfo::WaitItem& wait) {
				os << make_pair("type", "Wait") << ", "
				<< make_pair("stop_name", wait.stop->name.c_str()) << ", "
				<< make_pair("time", wait.time);
			}
		};
		visit(processor{ .os = os }, item);
		os << '}';
	}
	os << ']';
}

} // namespace

void ProcessRequests(
	std::ostream& os,
	const Descriptions& desc,
	const TransportRouter& router,
	const TransportMap& transport_map,
	const Json::Array& requests
) {
	os << '[';

	const char* left_brace = "{";
	for (const Json::Node& node : requests) {
		os << left_brace;
		left_brace = ", {";

		const Json::Dict& dict = node.AsMap();
		os << make_pair("request_id", dict.at("id").AsInt()) << ", ";

		const string& type = dict.at("type").AsString();
		if (type == "Bus") {
			ProcessBus(os, desc.GetBus(dict.at("name").AsString()));
		} else if (type == "Stop") {
			ProcessStop(os, desc.GetStop(dict.at("name").AsString()));
		} else if (type == "Route") {
			ProcessRoute(os, router, dict);
		} else if (type == "Map") {
			os << "\"map\": ";
			transport_map.Render(os);
		} else {
			throw invalid_argument("invalid request type - "s + type);
		}

		os << '}';
	}

	os << ']';
}
