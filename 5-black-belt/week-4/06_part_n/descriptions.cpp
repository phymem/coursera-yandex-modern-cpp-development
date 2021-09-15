#include "descriptions.h"

using namespace std;

Descriptions::Descriptions(
	const Json::Array& base_requests
) {
	items_.reserve(base_requests.size());

	ProcStops(base_requests);

	ProcBuses(base_requests);
}

void Descriptions::ProcStops(
	const Json::Array& base_requests
) {
	Graph::VertexId vertex_id = 0;
	for (const Json::Node& node : base_requests) {
		const Json::Dict& dict = node.AsMap();
		if (dict.at("type").AsString() == "Stop") {
			items_.push_back( Stop{
				.stop_id = static_cast<Handle>(stops_.size()),
				.json_dict = &dict,
				.name = dict.at("name").AsString(),
				.position = Sphere::Point{
					.latitude = dict.at("latitude").AsDouble(),
					.longitude = dict.at("longitude").AsDouble(),
				},
			});
			Stop* stop = &get<Stop>(items_.back());
			stops_dict_[stop->name] = stop;
			stops_.push_back(stop);
			stop->vertex_id_in = vertex_id++;
			stop->vertex_id_out = vertex_id++;
		}
	}

	for (Stop* stop : stops_) {
		if (stop->json_dict->count("road_distances")) {
			for (const auto& p : stop->json_dict->at("road_distances").AsMap()) {
				stop->distances[stops_dict_.at(p.first)->stop_id] = p.second.AsInt();
			}
		}
	}
}

void Descriptions::ProcBuses(
	const Json::Array& base_requests
) {
	auto compute_road_route_length = [](const StopsVec& stops) {
		int result = 0;
		for (size_t i = 1; i < stops.size(); ++i) {
			result += Descriptions::ComputeStopsDistance(
				*stops[i - 1],
				*stops[i]
			);
		}
		return result;
	};

	auto compute_geo_route_distance = [](const StopsVec& stops) {
		double result = 0;
		for (size_t i = 1; i < stops.size(); ++i) {
			result += Sphere::Distance(
				stops[i - 1]->position,
				stops[i]->position
			);
		}
		return result;
	};

	for (const Json::Node& node : base_requests) {
		const Json::Dict& dict = node.AsMap();
		if (dict.at("type").AsString() == "Bus") {
			items_.push_back( Bus{
				.bus_id = static_cast<Handle>(buses_.size()),
				.json_dict = &dict,
				.name = dict.at("name").AsString(),
				.is_roundtrip = dict.at("is_roundtrip").AsBool(),
			});
			Bus* bus = &get<Bus>(items_.back());
			buses_dict_[bus->name] = bus;
			buses_.push_back(bus);
			const Json::Array& stop_nodes = dict.at("stops").AsArray();
			bus->stops.reserve(stop_nodes.size());
			unordered_map<Stop*, unsigned int> stops_map;
			for (const Json::Node& stop_node : stop_nodes) {
				Stop* stop = stops_dict_.at(stop_node.AsString());
				bus->stops.push_back(stop);
				stops_map[stop]++;
				stop->buses[bus->name] = bus;
			}
			bus->stops.front()->is_base.insert(bus->bus_id);
			bus->unique_stop_count = static_cast<int>(stops_map.size());
			if (!bus->is_roundtrip && bus->stops.size() > 1) {
				bus->stops.back()->is_base.insert(bus->bus_id);
				bus->stops.reserve(stop_nodes.size() * 2 - 1);  // end stop is not repeated
				for (size_t stop_idx = stop_nodes.size() - 1; stop_idx > 0; --stop_idx) {
					Stop* stop = bus->stops[stop_idx - 1];
					bus->stops.push_back(stop);
					stops_map[stop]++;
				}
			}
			bus->road_route_length = compute_road_route_length(bus->stops);
			bus->geo_route_distance = compute_geo_route_distance(bus->stops);
			for (size_t i = 1; i < bus->stops.size(); ++i) {
				Stop* s1 = bus->stops[i - 1];
				Stop* s2 = bus->stops[i];
				s1->neighbour.insert(s2);
				s2->neighbour.insert(s1);
			}
			for (const auto& p : stops_map) {
				// if bus visits stop more than 2 times, then it's a base stop
				if (p.second > 2) {
					p.first->is_base.insert(bus->bus_id);
				}
			}
		}
	}
}
