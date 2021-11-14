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

Json::Dict ProcessBus(const Descriptions::Bus* bus) {
	if (!bus) {
		return Json::Dict{ {"error_message", Json::Node("not found")} };
	}
	return Json::Dict{
		{ "stop_count", Json::Node(static_cast<int>(bus->stops.size())) },
		{ "unique_stop_count", Json::Node(bus->unique_stop_count) },
		{ "route_length", Json::Node(bus->road_route_length) },
		{ "curvature", Json::Node(bus->road_route_length / bus->geo_route_distance) }
	};
}

Json::Dict ProcessStop(const Descriptions::Stop* stop) {
	if (!stop) {
		return Json::Dict{ {"error_message", Json::Node("not found")} };
	}
	Json::Array buses;
	buses.reserve(stop->buses.size());
	for (const auto& [_, bus] : stop->buses) {
		buses.emplace_back(bus->name);
	}
	Json::Dict dict;
	dict["buses"] = std::move(buses);
	return dict;
}

void ProcessRoute(
	ostream& os,
	const TransportRouter& router,
	const TransportMap& transport_map,
	const Json::Dict& dict
) {
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
	os << "], \"map\": ";
	//AG- transport_map.Render(os, &(*route));
}

} // namespace

Json::Array ProcessRequests(
	const Descriptions& desc,
	const TransportRouter& router,
	const TransportMap& transport_map,
	const Json::Array& requests
) {
	Json::Array responces;
	responces.reserve(requests.size());

	for (const Json::Node& req_node : requests) {
		const Json::Dict& req_dict = req_node.AsMap();
		const std::string req_type = req_dict.at("type").AsString();
		Json::Dict dict;
		if (req_type == "Bus") {
			dict = ProcessBus(desc.GetBus(dict.at("name").AsString()));
		} else if (req_type == "Stop") {
			dict = ProcessStop(desc.GetStop(dict.at("name").AsString()));
		} else if (req_type == "Route") {
			; // ProcessRoute(os, router, transport_map, dict);
		} else if (req_type == "Map") {
			dict = Json::Dict{ {"map", Json::Node(transport_map.Render(nullptr /*route*/)) } };
		} else {
			throw invalid_argument("invalid request type - "s + req_type);
		}
		dict["request_id"] = Json::Node(req_dict.at("id").AsInt());
		responces.push_back(std::move(dict));
	}

	return responces;
}
