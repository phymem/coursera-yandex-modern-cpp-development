#include "transport_router.h"

using namespace std;


TransportRouter::TransportRouter(
	const Descriptions& data,
	const Json::Dict& routing_settings_json)
		: descriptions_(&data), routing_settings_(MakeRoutingSettings(routing_settings_json))
{
	size_t edges_reserve = data.GetStops().size();
	for (const Descriptions::Bus* bus : data.GetBuses()) {
		size_t stops_count = bus->stops.size();
		edges_reserve += stops_count * ((1 + stops_count) / 2);
	}
	edges_info_.reserve(edges_reserve);

	const size_t vertex_count = data.GetStops().size() * 2;
	vertices_info_.resize(vertex_count);
	graph_ = BusGraph(vertex_count, edges_reserve);

	FillGraphWithStops(data.GetStops());
	FillGraphWithBuses(data.GetBuses());

	router_ = std::make_unique<Router>(graph_);
}

TransportRouter::RoutingSettings TransportRouter::MakeRoutingSettings(const Json::Dict& json) {
	return {
		json.at("bus_wait_time").AsInt(),
		json.at("bus_velocity").AsDouble(),
	};
}

void TransportRouter::FillGraphWithStops(const Descriptions::StopsVec& stops) {

	for (const Descriptions::Stop* stop : stops) {
		vertices_info_[stop->vertex_id_in] = { stop };
		vertices_info_[stop->vertex_id_out] = { stop };

		edges_info_.push_back(WaitEdgeInfo{});
		graph_.AddEdge({
			stop->vertex_id_out,
			stop->vertex_id_in,
			static_cast<double>(routing_settings_.bus_wait_time)
		});
	}

	assert(vertices_info_.size() == graph_.GetVertexCount());
}

void TransportRouter::FillGraphWithBuses(const Descriptions::BusesVec& buses) {
	for (const Descriptions::Bus* bus : buses) {
		const size_t stop_count = bus->stops.size();
		if (stop_count <= 1) {
			continue;
		}
		auto compute_distance_from = [bus](size_t lhs_idx) {
			return Descriptions::ComputeStopsDistance(
				*bus->stops[lhs_idx],
				*bus->stops[lhs_idx + 1]
			);
		};
		for (size_t start_stop_idx = 0; start_stop_idx + 1 < stop_count; ++start_stop_idx) {
			const Graph::VertexId start_vertex = bus->stops[start_stop_idx]->vertex_id_in;
			int total_distance = 0;
			for (size_t finish_stop_idx = start_stop_idx + 1; finish_stop_idx < stop_count; ++finish_stop_idx) {
				total_distance += compute_distance_from(finish_stop_idx - 1);
				edges_info_.push_back(BusEdgeInfo{
					.bus = bus,
					.span_count = finish_stop_idx - start_stop_idx,
				});
				graph_.AddEdge({
					start_vertex,
					bus->stops[finish_stop_idx]->vertex_id_out,
					total_distance * 1.0 / (routing_settings_.bus_velocity * 1000.0 / 60)  // m / (km/h * 1000 / 60) = min
				});
			}
		}
	}
}

optional<TransportRouter::RouteInfo> TransportRouter::FindRoute(const string& stop_from, const string& stop_to) const {
	const Graph::VertexId vertex_from = descriptions_->GetStop(stop_from)->vertex_id_out;
	const Graph::VertexId vertex_to = descriptions_->GetStop(stop_to)->vertex_id_out;
	const auto route = router_->BuildRoute(vertex_from, vertex_to);
	if (!route) {
		return nullopt;
	}

	RouteInfo route_info = {.total_time = route->weight};
	route_info.items.reserve(route->edge_count);
	for (size_t edge_idx = 0; edge_idx < route->edge_count; ++edge_idx) {
		const Graph::EdgeId edge_id = router_->GetRouteEdge(route->id, edge_idx);
		const auto& edge = graph_.GetEdge(edge_id);
		const auto& edge_info = edges_info_[edge_id];
		if (holds_alternative<BusEdgeInfo>(edge_info)) {
			const BusEdgeInfo& bus_edge_info = get<BusEdgeInfo>(edge_info);
			route_info.items.push_back(RouteInfo::BusItem{
				.bus = bus_edge_info.bus,
				.time = edge.weight,
				.span_count = bus_edge_info.span_count,
			});
		} else {
			const Graph::VertexId vertex_id = edge.from;
			route_info.items.push_back(RouteInfo::WaitItem{
				.stop = vertices_info_[vertex_id].stop,
				.time = edge.weight,
			});
		}
	}

	// Releasing in destructor of some proxy object would be better,
	// but we do not expect exceptions in normal workflow
	router_->ReleaseRoute(route->id);
	return route_info;
}
