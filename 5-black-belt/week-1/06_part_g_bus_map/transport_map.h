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

	void ProcStopsAndLabels(
		const Descriptions& desc,
		const Json::Dict* render_settings
	);

	void ProcRoutes(
		const Descriptions& desc,
		const Json::Dict* render_settings
	);

	std::vector<Svg::Polyline> routes_layer_;
	std::vector<Svg::Circle> stops_layer_;
	std::vector<Svg::Text> labels_layer_;
};


