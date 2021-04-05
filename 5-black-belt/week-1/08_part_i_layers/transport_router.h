#pragma once

#include "descriptions.h"
#include "graph.h"
#include "json.h"
#include "router.h"

#include <memory>
#include <vector>

class TransportRouter {
private:
	using BusGraph = Graph::DirectedWeightedGraph<double>;
	using Router = Graph::Router<double>;

public:
	TransportRouter(
		const Descriptions& data,
		const Json::Dict& routing_settings_json);

	struct RouteInfo {
		double total_time;

		struct BusItem {
			const Descriptions::Bus* bus;
			double time;
			size_t span_count;
		};
		struct WaitItem {
			const Descriptions::Stop* stop;
			double time;
		};

		using Item = std::variant<BusItem, WaitItem>;
		std::vector<Item> items;
	};

	std::optional<RouteInfo> FindRoute(const std::string& stop_from, const std::string& stop_to) const;

private:
	struct RoutingSettings {
		int bus_wait_time;  // in minutes
		double bus_velocity;  // km/h
	};

	static RoutingSettings MakeRoutingSettings(const Json::Dict& json);

	void FillGraphWithStops(const Descriptions::StopsVec& stops);

	void FillGraphWithBuses(const Descriptions::BusesVec& buses);

	struct VertexInfo {
		const Descriptions::Stop* stop;
	};

	struct BusEdgeInfo {
		const Descriptions::Bus* bus;
		size_t span_count;
	};
	struct WaitEdgeInfo {};
	using EdgeInfo = std::variant<BusEdgeInfo, WaitEdgeInfo>;

	const Descriptions* descriptions_;
	RoutingSettings routing_settings_;
	BusGraph graph_;
	std::unique_ptr<Router> router_;
	std::vector<VertexInfo> vertices_info_;
	std::vector<EdgeInfo> edges_info_;
};
