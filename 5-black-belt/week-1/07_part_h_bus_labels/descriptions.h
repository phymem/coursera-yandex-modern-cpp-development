#pragma once

#include "svg.h"
#include "json.h"
#include "graph.h"
#include "sphere.h"

#include <map>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

class Descriptions {
public:

	using Handle = unsigned int;

	struct Bus;

	struct Stop {
		Handle stop_id;
		const Json::Dict* json_dict;
		std::string name;
		Sphere::Point sphere_pos;
		Svg::Point svg_pos;

		std::map<std::string_view, Bus*> buses;

		// map by stop_id
		std::unordered_map<Handle, int> distances;

		// to speedup TransportRouter
		Graph::VertexId vertex_id_in;
		Graph::VertexId vertex_id_out;
	};
	using StopsVec = std::vector<Stop*>;

	struct Bus {
		Handle bus_id;
		const Json::Dict* json_dict;
		std::string name;
		StopsVec stops;
		bool is_roundtrip;
		int unique_stop_count;
		int road_route_length;
		double geo_route_distance;
	};
	using BusesVec = std::vector<Bus*>;

	Descriptions(
		const Json::Array& base_requests,
		const Json::Dict* render_settings
	);

	const BusesVec& GetBuses() const { return buses_; }
	const StopsVec& GetStops() const { return stops_; }

	const Stop* GetStop(const std::string& name) const {
		if (auto it = stops_dict_.find(name); it != stops_dict_.end())
			return it->second;
		return nullptr;
	}
	const Bus* GetBus(const std::string& name) const {
		if (auto it = buses_dict_.find(name); it != buses_dict_.end())
			return it->second;
		return nullptr;
	}

	static inline int ComputeStopsDistance(const Stop& lhs, const Stop& rhs) {
		if (auto it = lhs.distances.find(rhs.stop_id); it != lhs.distances.end())
			return it->second;
		return rhs.distances.at(lhs.stop_id);
	}

private:

	void ProcStops(const Json::Array& base_requests);

	void ProcBuses(const Json::Array& base_requests);

	void PrepRender(const Json::Dict& render_settings);

	using VariantVec = std::vector<std::variant<Stop, Bus>>;

	VariantVec items_;

	StopsVec stops_;
	BusesVec buses_;

	std::unordered_map<std::string, Stop*> stops_dict_;
	std::unordered_map<std::string, Bus*> buses_dict_;
};
