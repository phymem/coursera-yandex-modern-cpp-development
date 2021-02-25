#include "transport_catalog.h"

using namespace std;

inline int ComputeRoadRouteLength(
	const std::vector<std::string>& stops,
	const Descriptions::StopsDict& stops_dict
) {
	int result = 0;
	for (size_t i = 1; i < stops.size(); ++i) {
		result += Descriptions::ComputeStopsDistance(
			*stops_dict.at(stops[i - 1]),
			*stops_dict.at(stops[i])
		);
	}
	return result;
}

inline double ComputeGeoRouteDistance(
	const std::vector<std::string>& stops,
	const Descriptions::StopsDict& stops_dict
) {
	double result = 0;
	for (size_t i = 1; i < stops.size(); ++i) {
		result += Sphere::Distance(
			stops_dict.at(stops[i - 1])->position,
			stops_dict.at(stops[i])->position
		);
	}
	return result;
}

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

TransportCatalog::TransportCatalog(
	vector<Descriptions::InputQuery> data,
	const Json::Dict& routing_settings,
	const Json::Dict* render_settings
) {
	auto stops_end = partition(begin(data), end(data), [](const auto& item) {
		return holds_alternative<Descriptions::Stop>(item);
	});

	Descriptions::StopsDict stops_dict;
	for (const auto& item : Range{begin(data), stops_end}) {
		const auto& stop = get<Descriptions::Stop>(item);
		stops_dict[stop.name] = &stop;
		stops_.insert({stop.name, {}});
	}

	Descriptions::BusesDict buses_dict;
	for (const auto& item : Range{stops_end, end(data)}) {
		const auto& bus = get<Descriptions::Bus>(item);

		buses_dict[bus.name] = &bus;
		buses_[bus.name] = Responses::Bus{
			bus.stops.size(),
			ComputeUniqueItemsCount(AsRange(bus.stops)),
			ComputeRoadRouteLength(bus.stops, stops_dict),
			ComputeGeoRouteDistance(bus.stops, stops_dict)
		};

		for (const string& stop_name : bus.stops) {
			stops_.at(stop_name).bus_names.insert(bus.name);
		}
	}

	router_ = make_unique<TransportRouter>(stops_dict, buses_dict, routing_settings);

	if (render_settings) {
		to_svg_point_.ComputeZoomCoef(begin(data), stops_end, render_settings);

		CreateStopsAndLabelsLayer(begin(data), stops_end, render_settings);

		CreateRoutesLayer(stops_end, end(data), stops_dict, render_settings);
	}
}

void TransportCatalog::ToSvgPoint::ComputeZoomCoef(
	vector<Descriptions::InputQuery>::const_iterator stops_begin,
	vector<Descriptions::InputQuery>::const_iterator stops_end,
	const Json::Dict* render_settings
) {
	bool init_min_max = true;
	for (const auto& item : Range{stops_begin, stops_end} ) {
		Sphere::Point pos = get<Descriptions::Stop>(item).position;
		if (init_min_max) {
			init_min_max = false;
			max_lon = min_lon = pos.longitude;
			max_lat = min_lat = pos.latitude;
		}
		else {
			max_lon = max(max_lon, pos.longitude);
			min_lon = min(min_lon, pos.longitude);
			max_lat = max(max_lat, pos.latitude);
			min_lat = min(min_lat, pos.latitude);
		}
	}

	padding = render_settings->at("padding").AsDouble();

	double w_coef = 0;
	if (max_lon != min_lon) {
		w_coef = (render_settings->at("width").AsDouble() - 2 * padding) / (max_lon - min_lon);
	}
	double h_coef = 0;
	if (max_lat != min_lat) {
		h_coef = (render_settings->at("height").AsDouble() - 2 * padding) / (max_lat - min_lat);
	}

	if (w_coef && h_coef) {
		zoom_coef = min(w_coef, h_coef);
	}
	else {
		zoom_coef = w_coef ? w_coef : h_coef;
	}
}

void TransportCatalog::CreateStopsAndLabelsLayer(
	vector<Descriptions::InputQuery>::const_iterator stops_begin,
	vector<Descriptions::InputQuery>::const_iterator stops_end,
	const Json::Dict* render_settings
) {
	unsigned int stops_count = distance(stops_begin, stops_end);

	vector<const Descriptions::Stop*> stops_vec;
	stops_vec.reserve(stops_count);
	for (const auto& item : Range{stops_begin, stops_end} ) {
		stops_vec.push_back(&get<Descriptions::Stop>(item));
	}
	sort(begin(stops_vec), end(stops_vec),
		[](const Descriptions::Stop* lhs, const Descriptions::Stop* rhs)
		{ return  lhs->name < rhs->name; });

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

	stops_layer_.reserve(stops_count);
	labels_layer_.reserve(stops_count);
	for (const Descriptions::Stop* stop : stops_vec) {
		Svg::Point pnt = to_svg_point_(stop->position);

		stops_layer_.push_back(
			Svg::Circle{}
				.SetRadius(stop_radius)
				.SetFillColor("white")
				.SetCenter(pnt)
		);

		label_template.SetData(stop->name);
		label_template.SetPoint(pnt);

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

void TransportCatalog::CreateRoutesLayer(
	vector<Descriptions::InputQuery>::const_iterator buses_begin,
	vector<Descriptions::InputQuery>::const_iterator buses_end,
	const Descriptions::StopsDict& stops_dict,
	const Json::Dict* render_settings
) {
	unsigned int buses_count = distance(buses_begin, buses_end);

	vector<const Descriptions::Bus*> buses_vec;
	buses_vec.reserve(buses_count);
	for (const auto& item : Range{buses_begin, buses_end}) {
		buses_vec.push_back(&get<Descriptions::Bus>(item));
	}
	sort(begin(buses_vec), end(buses_vec),
		[](const Descriptions::Bus* lhs, const Descriptions::Bus* rhs)
		{ return  lhs->name < rhs->name; });

	const Json::Array& json_palette = render_settings->at("color_palette").AsArray();
	vector<Svg::Color> palette;

	palette.reserve(json_palette.size());
	for (const auto& node : json_palette) {
		palette.push_back(AsColor(node));
	}

	double line_width = render_settings->at("line_width").AsDouble();

	unsigned int current_color = 0;
	routes_layer_.reserve(buses_count);
	for (const Descriptions::Bus* bus : buses_vec) {
		Svg::Polyline line;
		line.SetStrokeColor(palette[current_color++ % palette.size()])
			.SetStrokeWidth(line_width)
			.SetStrokeLineCap("round")
			.SetStrokeLineJoin("round");
		for (const string& stop_name : bus->stops) {
			line.AddPoint(to_svg_point_(stops_dict.at(stop_name)->position));
		}
		routes_layer_.push_back(std::move(line));
	}
}
