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
		Svg::Color underlayer_color = AsColor(render_settings->at("underlayer_color"));

		ProcStops(desc.GetStops(), render_settings, underlayer_color);

		ProcBuses(desc.GetBuses(), render_settings, underlayer_color);
	}
}

void TransportMap::ProcStops(
	const Descriptions::StopsVec& stops,
	const Json::Dict* render_settings,
	const Svg::Color& underlayer_color
) {
	const Json::Array& label_offset = render_settings->at("stop_label_offset").AsArray();
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

	stops_layer_.reserve(stops.size());
	stop_labels_layer_.reserve(stops.size());
	for (const Descriptions::Stop* stop : stops) {
		stops_layer_.push_back(
			Svg::Circle{}
				.SetRadius(stop_radius)
				.SetFillColor("white")
				.SetCenter(stop->svg_pos)
		);

		label_template.SetData(stop->name);
		label_template.SetPoint(stop->svg_pos);

		stop_labels_layer_.push_back(
			Svg::Text{ label_template }
				.SetFillColor(underlayer_color)
				.SetStrokeColor(underlayer_color)
				.SetStrokeWidth(underlayer_width)
				.SetStrokeLineCap("round")
				.SetStrokeLineJoin("round")
		);
		stop_labels_layer_.push_back(label_template);
	}
}

void TransportMap::ProcBuses(
	const Descriptions::BusesVec& buses,
	const Json::Dict* render_settings,
	const Svg::Color& underlayer_color
) {
	const Json::Array& json_palette = render_settings->at("color_palette").AsArray();
	const Json::Array& label_offset = render_settings->at("bus_label_offset").AsArray();
	double underlayer_width = render_settings->at("underlayer_width").AsDouble();
	double line_width = render_settings->at("line_width").AsDouble();
	vector<Svg::Color> palette;

	palette.reserve(json_palette.size());
	for (const auto& node : json_palette) {
		palette.push_back(AsColor(node));
	}

	Svg::Text label_template = Svg::Text{}
		.SetFillColor("black")
		.SetFontFamily("Verdana")
		.SetFontWeight("bold")
		.SetFontSize(render_settings->at("bus_label_font_size").AsInt())
		.SetOffset({
			label_offset.at(0 /*offset_x*/).AsDouble(),
			label_offset.at(1 /*offset_y*/).AsDouble()
		});

	Svg::Text underlayer_template = Svg::Text{ label_template }
		.SetFillColor(underlayer_color)
		.SetStrokeColor(underlayer_color)
		.SetStrokeWidth(underlayer_width)
		.SetStrokeLineCap("round")
		.SetStrokeLineJoin("round");

	unsigned int current_color = 0;
	routes_layer_.reserve(buses.size());
	for (const Descriptions::Bus* bus : buses) {

		Svg::Polyline line;
		line.SetStrokeColor(palette[current_color])
			.SetStrokeWidth(line_width)
			.SetStrokeLineCap("round")
			.SetStrokeLineJoin("round");
		for (const Descriptions::Stop* stop : bus->stops) {
			line.AddPoint(stop->svg_pos);
		}
		routes_layer_.push_back(std::move(line));

		const Descriptions::Stop* first_stop = bus->stops.front();
		bus_labels_layer_.push_back(
			underlayer_template
				.SetData(bus->name)
				.SetPoint(first_stop->svg_pos)
		);
		bus_labels_layer_.push_back(
			label_template
				.SetFillColor(palette[current_color])
				.SetData(bus->name)
				.SetPoint(first_stop->svg_pos)
		);

		if (!bus->is_roundtrip) {
			const Descriptions::Stop* last_stop = bus->stops[bus->stops.size() / 2];
			if (first_stop != last_stop) {
				bus_labels_layer_.push_back(
					underlayer_template.SetPoint(last_stop->svg_pos));
				bus_labels_layer_.push_back(
					label_template.SetPoint(last_stop->svg_pos));
			}
		}

		current_color = (current_color + 1) % palette.size();
	}
}

void TransportMap::Render(ostream& os) const {
	os << "\"<?xml version=\\\"1.0\\\" encoding=\\\"UTF-8\\\" ?>"
		"<svg xmlns=\\\"http://www.w3.org/2000/svg\\\" version=\\\"1.1\\\">";

	for_each(begin(routes_layer_), end(routes_layer_),
		[&os](const Svg::Polyline& line){ os << line; });

	for_each(begin(bus_labels_layer_), end(bus_labels_layer_),
		[&os](const Svg::Text& label){ os << label; });

	for_each(begin(stops_layer_), end(stops_layer_),
		[&os](const Svg::Circle& circle){ os << circle; });

	for_each(begin(stop_labels_layer_), end(stop_labels_layer_),
		[&os](const Svg::Text& label){ os << label; });

	os << "</svg>\"";
}
