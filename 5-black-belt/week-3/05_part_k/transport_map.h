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

	enum LayerType {
		LT_BUS_LINES,
		LT_BUS_LABELS,
		LT_STOP_POINTS,
		LT_STOP_LABELS
	};

	std::vector<LayerType> layers_;

	std::vector<Svg::Polyline> bus_lines_;
	std::vector<Svg::Circle> stop_points_;
	std::vector<Svg::Text> stop_labels_;
	std::vector<Svg::Text> bus_labels_;
};


