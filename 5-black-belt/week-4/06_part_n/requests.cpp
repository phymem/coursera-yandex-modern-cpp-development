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
		return Json::Dict{ { "error_message", Json::Node("not found") } };
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
		return Json::Dict{ { "error_message", Json::Node("not found") } };
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

Json::Dict ProcessRoute(
	const TransportRouter& router,
	const TransportMap& transport_map,
	const Json::Dict& req_dict
) {
	const auto route = router.FindRoute(
		req_dict.at("from").AsString(),
		req_dict.at("to").AsString()
	);
	if (!route) {
		return Json::Dict{ { "error_message", Json::Node("not found") } };
	}
	Json::Array items;
	items.reserve(route->items.size());
	for (const auto& it : route->items) {
		struct processor {
			Json::Dict operator() (const TransportRouter::RouteInfo::BusItem& bus) {
				return Json::Dict{
					{ "type", Json::Node("Bus") },
					{ "bus", Json::Node(bus.bus->name) },
					{ "time", Json::Node(bus.time) },
					{ "span_count", Json::Node(static_cast<int>(bus.span_count)) }
				};
			}
			Json::Dict operator() (const TransportRouter::RouteInfo::WaitItem& wait) {
				return Json::Dict{
					{ "type", Json::Node("Wait") },
					{ "stop_name", Json::Node(wait.stop->name) },
					{ "time", Json::Node(wait.time) }
				};
			}
		};
		items.push_back(visit(processor{}, it));
	}
	Json::Dict dict;
	dict["items"] = std::move(items);
	dict["total_time"] = Json::Node(route->total_time);
	dict["map"] = Json::Node(transport_map.Render(&(*route)));
	return dict;
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

	for (const Json::Node& req : requests) {
		const std::string req_type = req.AsMap().at("type").AsString();
		Json::Dict dict;
		if (req_type == "Bus") {
			dict = ProcessBus(desc.GetBus(req.AsMap().at("name").AsString()));
		} else if (req_type == "Stop") {
			dict = ProcessStop(desc.GetStop(req.AsMap().at("name").AsString()));
		} else if (req_type == "Route") {
			dict = ProcessRoute(router, transport_map, req.AsMap());
		} else if (req_type == "Map") {
			dict = Json::Dict{ {"map", Json::Node(transport_map.Render(nullptr /*route*/)) } };
		} else {
			throw invalid_argument("invalid request type - "s + req_type);
		}
		dict["request_id"] = Json::Node(req.AsMap().at("id").AsInt());
		responces.push_back(std::move(dict));
	}

	return responces;
}
