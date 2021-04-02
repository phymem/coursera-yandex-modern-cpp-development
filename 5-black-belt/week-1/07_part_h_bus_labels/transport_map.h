#pragma once

#include "descriptions.h"
#include "json.h"
#include "svg.h"

#include <string>
#include <vector>
#include <iostream>

class TransportMap {
public:
	TransportMap(
		const Descriptions& desc,
		const Json::Dict* render_settings);

	void Render(std::ostream& os) const;

private:

	void ProcStops(
		const Descriptions::StopsVec& stops,
		const Json::Dict* render_settings,
		const Svg::Color& underlayer_color
	);

	void ProcBuses(
		const Descriptions::BusesVec& buses,
		const Json::Dict* render_settings,
		const Svg::Color& underlayer_color
	);

	std::vector<Svg::Polyline> routes_layer_;
	std::vector<Svg::Circle> stops_layer_;
	std::vector<Svg::Text> stop_labels_layer_;
	std::vector<Svg::Text> bus_labels_layer_;
};


