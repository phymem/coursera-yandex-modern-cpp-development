#pragma once

#include "descriptions.h"
#include "transport_router.h"
#include "json.h"
#include "svg.h"

#include <string>
#include <vector>
#include <iostream>

class TransportMap {
public:
	TransportMap(
		const Descriptions& desc,
		const Json::Dict& render_settings);

	void Render(std::ostream& os, const TransportRouter::RouteInfo* route) const;

private:

	void CalcStopPos();

	void ProcStops();
	void ProcBuses();

	void RenderRoute(std::ostream& os, const TransportRouter::RouteInfo* route) const;

	struct RenderSettings {
		double width;
		double height;
		double padding;
		double outer_margin;
		double stop_radius;
		double line_width;

		struct Label {
			int font_size;
			double offset_x;
			double offset_y;
		};
		Label bus_label;
		Label stop_label;

		struct Underlayer {
			Svg::Color color;
			double width;
		};
		Underlayer underlayer;

		std::vector<Svg::Color> palette;

		enum LayerType {
			LT_BUS_LINES,
			LT_BUS_LABELS,
			LT_STOP_POINTS,
			LT_STOP_LABELS
		};
		std::vector<LayerType> layers;

		void Init(const Json::Dict& render_settings);
	};
	RenderSettings render_settings_;

	struct Stop {
		const Descriptions::Stop* desc;
		Sphere::Point map_pos;
		Svg::Point svg_pos;
		int pos_idx;

		Svg::Circle circle;
		Svg::Text label_top;
		Svg::Text label_bot;
	};
	std::vector<Stop> stop_items_;
	std::vector<Stop*> stops_;

	struct Bus {
		const Descriptions::Bus* desc;

		const Stop* first_stop = nullptr;
		const Stop* last_stop = nullptr;

		Svg::Color color;
		Svg::Polyline line;
		Svg::Text label_top;
		Svg::Text label_bot;
	};
	std::vector<Bus> bus_items_;
	std::vector<Bus*> buses_;
};
