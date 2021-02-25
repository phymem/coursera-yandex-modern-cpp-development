#pragma once

#include "descriptions.h"
#include "json.h"
#include "transport_router.h"
#include "utils.h"
#include "svg.h"

#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <sstream>

namespace Responses {
	struct Stop {
		std::set<std::string> bus_names;
	};

	struct Bus {
		size_t stop_count = 0;
		size_t unique_stop_count = 0;
		int road_route_length = 0;
		double geo_route_length = 0.0;
	};
}

class TransportCatalog {
public:
	TransportCatalog(
		std::vector<Descriptions::InputQuery> data,
		const Json::Dict& routing_settings,
		const Json::Dict* render_settings);

	const Responses::Stop* GetStop(const std::string& name) const {
		if (auto it = stops_.find(name); it != stops_.end())
			return &it->second;
		return nullptr;
	}
	const Responses::Bus* GetBus(const std::string& name) const {
		if (auto it = buses_.find(name); it != buses_.end())
			return &it->second;
		return nullptr;
	}

	std::optional<TransportRouter::RouteInfo>
	FindRoute(const std::string& stop_from, const std::string& stop_to) const {
		return router_->FindRoute(stop_from, stop_to);
	}

	std::string RenderMap() const;

private:
	void CreateStopsAndLabelsLayer(
		std::vector<Descriptions::InputQuery>::const_iterator stops_begin,
		std::vector<Descriptions::InputQuery>::const_iterator stops_end,
		const Json::Dict* render_settings
	);

	void CreateRoutesLayer(
		std::vector<Descriptions::InputQuery>::const_iterator buses_begin,
		std::vector<Descriptions::InputQuery>::const_iterator buses_end,
		const Descriptions::StopsDict& stops_dict,
		const Json::Dict* render_settings
	);

	struct ToSvgPoint {
		double max_lon, min_lon;
		double max_lat, min_lat;

		double zoom_coef;
		double padding;

		void ComputeZoomCoef(
			std::vector<Descriptions::InputQuery>::const_iterator stops_begin,
			std::vector<Descriptions::InputQuery>::const_iterator stops_end,
			const Json::Dict* render_settings);

		Svg::Point operator() (const Sphere::Point& pnt) const {
			return Svg::Point{
				.x = (pnt.longitude - min_lon) * zoom_coef + padding,
				.y = (max_lat - pnt.latitude) * zoom_coef + padding
			};
		}
	};
	ToSvgPoint to_svg_point_;

	std::unordered_map<std::string, Responses::Stop> stops_;
	std::unordered_map<std::string, Responses::Bus> buses_;
	std::unique_ptr<TransportRouter> router_;

	std::vector<Svg::Polyline> routes_layer_;
	std::vector<Svg::Circle> stops_layer_;
	std::vector<Svg::Text> labels_layer_;
};

inline std::string TransportCatalog::RenderMap() const {
	Svg::Document doc;

	std::for_each(begin(routes_layer_), end(routes_layer_),
		[&doc](const Svg::Polyline& line){ doc.Add(line); });

	std::for_each(begin(stops_layer_), end(stops_layer_),
		[&doc](const Svg::Circle& circle){ doc.Add(circle); });

	std::for_each(begin(labels_layer_), end(labels_layer_),
		[&doc](const Svg::Text& label){ doc.Add(label); });

	std::ostringstream oss;
	doc.Render(oss);
	return std::move(oss.str());
}
