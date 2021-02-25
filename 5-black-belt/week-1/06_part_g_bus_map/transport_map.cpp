#include "transport_map.h"

#include <cassert>
#include <algorithm>

using namespace std;

inline Svg::Color AsColor(
	const Json::Node& node
) {
	if (std::holds_alternative<std::string>(node.GetBase())) {
		return Svg::Color{
			std::get<std::string>(node.GetBase())
		};
	}

	enum { RGB_R, RGB_G, RGB_B, RGB_SIZE, RGBA_A = RGB_SIZE, RGBA_SIZE };

	const Json::Array& arr = std::get<Json::Array>(node.GetBase());
	if (arr.size() == RGB_SIZE) {
		return Svg::Color{ Svg::Rgb{
			static_cast<unsigned char>(arr[RGB_R].AsInt()),
			static_cast<unsigned char>(arr[RGB_G].AsInt()),
			static_cast<unsigned char>(arr[RGB_B].AsInt())
		}};
	}

	assert(arr.size() == RGBA_SIZE);
	return Svg::Color{ Svg::Rgba{
		static_cast<unsigned char>(arr[RGB_R].AsInt()),
		static_cast<unsigned char>(arr[RGB_G].AsInt()),
		static_cast<unsigned char>(arr[RGB_B].AsInt()),
		arr[RGBA_A].AsDouble()
	}};
}

TransportMap::TransportMap(
	const Descriptions& desc,
	const Json::Dict* render_settings
) {
	if (render_settings) {
		ProcStopsAndLabels(desc, render_settings);

		ProcRoutes(desc, render_settings);
	}
}

void TransportMap::ProcStopsAndLabels(
	const Descriptions& desc,
	const Json::Dict* render_settings
) {
	const Json::Array& label_offset = render_settings->at("stop_label_offset").AsArray();
	Svg::Color underlayer_color = AsColor(render_settings->at("underlayer_color"));
	double underlayer_width = render_settings->at("underlayer_width").AsDouble();
	double stop_radius = render_settings->at("stop_radius").AsDouble();

	Svg::Text label_template = Svg::Text{}
		.SetFillColor("black")
		.SetFontFamily("Verdana")
		.SetFontSize(render_settings->at("stop_label_font_size").AsInt())
		.SetOffset({
			label_offset.at(0 /*offset_x*/).AsDouble(),
			label_offset.at(1 /*offset_y*/).AsDouble()
		});

	stops_layer_.reserve(desc.GetStops().size());
	labels_layer_.reserve(desc.GetStops().size());
	for (const Descriptions::Stop* stop : desc.GetStops()) {
		stops_layer_.push_back(
			Svg::Circle{}
				.SetRadius(stop_radius)
				.SetFillColor("white")
				.SetCenter(stop->svg_pos)
		);

		label_template.SetData(stop->name);
		label_template.SetPoint(stop->svg_pos);

		labels_layer_.push_back(
			Svg::Text{ label_template }
				.SetFillColor(underlayer_color)
				.SetStrokeColor(underlayer_color)
				.SetStrokeWidth(underlayer_width)
				.SetStrokeLineCap("round")
				.SetStrokeLineJoin("round")
		);
		labels_layer_.push_back(label_template);
	}
}

void TransportMap::ProcRoutes(
	const Descriptions& desc,
	const Json::Dict* render_settings
) {
	const Json::Array& json_palette = render_settings->at("color_palette").AsArray();
	vector<Svg::Color> palette;

	palette.reserve(json_palette.size());
	for (const auto& node : json_palette) {
		palette.push_back(AsColor(node));
	}

	double line_width = render_settings->at("line_width").AsDouble();

	unsigned int current_color = 0;
	routes_layer_.reserve(desc.GetBuses().size());
	for (const Descriptions::Bus* bus : desc.GetBuses()) {
		Svg::Polyline line;
		line.SetStrokeColor(palette[current_color++ % palette.size()])
			.SetStrokeWidth(line_width)
			.SetStrokeLineCap("round")
			.SetStrokeLineJoin("round");
		for (const Descriptions::Stop* stop : bus->stops) {
			line.AddPoint(stop->svg_pos);
		}
		routes_layer_.push_back(std::move(line));
	}
}

void TransportMap::Render(ostream& os) const {
	os << "\"<?xml version=\\\"1.0\\\" encoding=\\\"UTF-8\\\" ?>"
		"<svg xmlns=\\\"http://www.w3.org/2000/svg\\\" version=\\\"1.1\\\">";

	for_each(begin(routes_layer_), end(routes_layer_),
		[&os](const Svg::Polyline& line){ os << line; });

	for_each(begin(stops_layer_), end(stops_layer_),
		[&os](const Svg::Circle& circle){ os << circle; });

	for_each(begin(labels_layer_), end(labels_layer_),
		[&os](const Svg::Text& label){ os << label; });

	os << "</svg>\"";
}
