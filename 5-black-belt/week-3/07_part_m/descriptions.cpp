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
			Sphere::Point pos {
				.latitude = dict.at("latitude").AsDouble(),
				.longitude = dict.at("longitude").AsDouble(),
			};
			items_.push_back( Stop{
				.stop_id = static_cast<Handle>(items_.size()),
				.json_dict = &dict,
				.name = dict.at("name").AsString(),
				.sphere_pos = pos,
				.graph_pos = pos
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

	for (Bus* bus : buses_) {
		auto prev = begin(bus->stops);
		auto curr = prev + 1;
		while (curr != end(bus->stops)) {
			if ((*curr)->buses.size() > 1
				|| (*curr)->is_base.count(bus->bus_id)) {
				if (curr - prev > 1) {
					double step_lon = ((*curr)->graph_pos.longitude
						- (*prev)->graph_pos.longitude) / (curr - prev);
					double step_lat = ((*curr)->graph_pos.latitude
						- (*prev)->graph_pos.latitude) / (curr - prev);
					for (auto it = prev; it <= curr; ++it) {
						(*it)->graph_pos.longitude = step_lon * (it - prev)
							+ (*prev)->graph_pos.longitude;
						(*it)->graph_pos.latitude = step_lat * (it - prev)
							+ (*prev)->graph_pos.latitude;
					}
				}
				prev = curr;
			}
			++curr;
		}
	}

	std::sort(std::begin(stops_), std::end(stops_),
		[](const Stop* lhs, const Stop* rhs)
		{ return lhs->graph_pos.longitude < rhs->graph_pos.longitude; });
	double x_step = (width - 2 * padding) / CalcGraphIdx();
	for (Stop* stop : stops_) {
		stop->svg_pos.x = stop->graph_idx * x_step + padding;
	}

	std::sort(std::begin(stops_), std::end(stops_),
		[](const Stop* lhs, const Stop* rhs)
		{ return lhs->graph_pos.latitude < rhs->graph_pos.latitude; });
	double y_step = (height - 2 * padding) / CalcGraphIdx();
	for (Stop* stop : stops_) {
		stop->svg_pos.y = height - padding - stop->graph_idx * y_step;
	}
}

int Descriptions::CalcGraphIdx() {
	for (Stop* stop : stops_) {
		stop->graph_idx = -1;
	}
	int max_idx = -1;
	for (Stop* stop : stops_) {
		for (Stop* neighbour : stop->neighbour) {
			stop->graph_idx = std::max(
				stop->graph_idx, neighbour->graph_idx);
		}
		stop->graph_idx++;
		max_idx = std::max(max_idx, stop->graph_idx);
	}
	return max_idx;
}
