#include "descriptions.h"

using namespace std;

Descriptions::Descriptions(
	const Json::Array& base_requests,
	const Json::Dict* render_settings
) {
	items_.reserve(base_requests.size());

	ProcStops(base_requests);

	ProcBuses(base_requests);

	if (render_settings) {
		PrepRender(*render_settings);

		sort(begin(buses_), end(buses_),
			[](const Bus* lhs, const Bus* rhs)
			{ return lhs->name < rhs->name; });

		sort(begin(stops_), end(stops_),
			[](const Stop* lhs, const Stop* rhs)
			{ return lhs->name < rhs->name; });
	}
}

void Descriptions::ProcStops(
	const Json::Array& base_requests
) {
	Graph::VertexId vertex_id = 0;
	for (const Json::Node& node : base_requests) {
		const Json::Dict& dict = node.AsMap();
		if (dict.at("type").AsString() == "Stop") {
			items_.push_back( Stop{
				.stop_id = static_cast<Handle>(items_.size()),
				.json_dict = &dict,
				.name = dict.at("name").AsString(),
				.sphere_pos = {
					.latitude = dict.at("latitude").AsDouble(),
					.longitude = dict.at("longitude").AsDouble(),
				}
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
				stops[i - 1]->sphere_pos,
				stops[i]->sphere_pos
			);
		}
		return result;
	};

	for (const Json::Node& node : base_requests) {
		const Json::Dict& dict = node.AsMap();
		if (dict.at("type").AsString() == "Bus") {
			items_.push_back( Bus{
				.bus_id = static_cast<Handle>(items_.size()),
				.json_dict = &dict,
				.name = dict.at("name").AsString(),
				.is_roundtrip = dict.at("is_roundtrip").AsBool(),
			});
			Bus* bus = &get<Bus>(items_.back());
			buses_dict_[bus->name] = bus;
			buses_.push_back(bus);
			const Json::Array& stop_nodes = dict.at("stops").AsArray();
			bus->stops.reserve(stop_nodes.size());
			unordered_set<Handle> unique_stops;
			for (const Json::Node& stop_node : stop_nodes) {
				Stop* stop = stops_dict_.at(stop_node.AsString());
				bus->stops.push_back(stop);
				unique_stops.insert(stop->stop_id);
				stop->buses[bus->name] = bus;
			}
			bus->unique_stop_count = static_cast<int>(unique_stops.size());
			if (!bus->is_roundtrip && bus->stops.size() > 1) {
				bus->stops.reserve(stop_nodes.size() * 2 - 1);  // end stop is not repeated
				for (size_t stop_idx = stop_nodes.size() - 1; stop_idx > 0; --stop_idx) {
					bus->stops.push_back(bus->stops[stop_idx - 1]);
				}
			}
			bus->road_route_length = compute_road_route_length(bus->stops);
			bus->geo_route_distance = compute_geo_route_distance(bus->stops);
			for (size_t i = 1; i < bus->stops.size(); ++i) {
				Stop* s1 = bus->stops[i - 1];
				Stop* s2 = bus->stops[i];
				s1->neighbour.insert(s2->stop_id);
				s2->neighbour.insert(s1->stop_id);
			}
		}
	}
}

void Descriptions::PrepRender(
	const Json::Dict& render_settings
) {
	double padding = render_settings.at("padding").AsDouble();
	double height = render_settings.at("height").AsDouble();
	double width = render_settings.at("width").AsDouble();

	if (stops_.size() == 1) {
		stops_.front()->svg_pos = Svg::Point{
			.x = padding,
			.y = height - padding
		};
		return;
	}

	CalcStopPosIdx([](const Stop* lhs, const Stop* rhs)
		{ return lhs->sphere_pos.longitude < rhs->sphere_pos.longitude; });
		double x_step = (width - 2 * padding) / stops_.back()->pos_idx;
	for (Stop* stop : stops_) {
		stop->svg_pos.x = stop->pos_idx * x_step + padding;
	}

	CalcStopPosIdx([](const Stop* lhs, const Stop* rhs)
		{ return lhs->sphere_pos.latitude < rhs->sphere_pos.latitude; });
	double y_step = (height - 2 * padding) / stops_.back()->pos_idx;
	for (Stop* stop : stops_) {
		stop->svg_pos.y = height - padding - stop->pos_idx * y_step;
	}
}
