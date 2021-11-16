#include "transport_map.h"

#include <cassert>
#include <sstream>
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
	const Json::Dict& render_settings
) {
	render_settings_.Init(render_settings);

	stop_items_.resize(desc.GetStops().size());
	stops_.reserve(stop_items_.size());
	for (const Descriptions::Stop* desc : desc.GetStops()) {
		Stop* stop = &stop_items_[desc->stop_id];
		stops_.push_back(stop);
		stop->desc = desc;
		stop->map_pos = desc->position;
	}

	bus_items_.resize(desc.GetBuses().size());
	buses_.reserve(bus_items_.size());
	for (const Descriptions::Bus* desc : desc.GetBuses()) {
		Bus* bus = &bus_items_[desc->bus_id];
		buses_.push_back(bus);
		bus->desc = desc;
		bus->first_stop = &stop_items_[desc->stops.front()->stop_id];
		if (!desc->is_roundtrip) {
			const Stop* stop = &stop_items_[desc->stops[desc->stops.size() / 2]->stop_id];
			if (bus->first_stop != stop) {
				bus->last_stop = stop;
			}
		}
	}

	CalcStopPos();

	sort(begin(buses_), end(buses_),
		[](const Bus* lhs, const Bus* rhs)
		{ return lhs->desc->name < rhs->desc->name; });

	sort(begin(stops_), end(stops_),
		[](const Stop* lhs, const Stop* rhs)
		{ return lhs->desc->name < rhs->desc->name; });

	ProcStops();

	ProcBuses();
}

void TransportMap::CalcStopPos() {
	if (stops_.size() == 1) {
		stops_.front()->svg_pos = Svg::Point{
			.x = render_settings_.padding,
			.y = render_settings_.height - render_settings_.padding
		};
		return;
	}

	for (const Bus* bus : buses_) {
		auto prev = begin(bus->desc->stops);
		auto curr = prev + 1;
		while (curr != end(bus->desc->stops)) {
			if ((*curr)->buses.size() > 1 || (*curr)->is_base.count(bus->desc->bus_id)) {
				if (curr - prev > 1) {
					Stop* prev_stop = &stop_items_[(*prev)->stop_id];
					Stop* curr_stop = &stop_items_[(*curr)->stop_id];
					double step_lon = (curr_stop->map_pos.longitude - prev_stop->map_pos.longitude) / (curr - prev);
					double step_lat = (curr_stop->map_pos.latitude - prev_stop->map_pos.latitude) / (curr - prev);
					for (auto it = prev; it <= curr; ++it) {
						stop_items_[(*it)->stop_id].map_pos = Sphere::Point{
							.longitude = step_lon * (it - prev) + prev_stop->map_pos.longitude,
							.latitude = step_lat * (it - prev) + prev_stop->map_pos.latitude
						};
					}
				}
				prev = curr;
			}
			++curr;
		}
	}

	auto calc_pos_idx = [this]() {
		for (Stop* stop : stops_) {
			stop->pos_idx = -1;
		}
		int max_idx = -1;
		for (Stop* stop : stops_) {
			for (const Descriptions::Stop* neighbour : stop->desc->neighbour) {
				stop->pos_idx = std::max(
					stop->pos_idx, stop_items_[neighbour->stop_id].pos_idx);
			}
			stop->pos_idx++;
			max_idx = std::max(max_idx, stop->pos_idx);
		}
		return max_idx;
	};

	std::sort(std::begin(stops_), std::end(stops_),
		[](const Stop* lhs, const Stop* rhs)
		{ return lhs->map_pos.longitude < rhs->map_pos.longitude; });
	double x_step = (render_settings_.width - 2 * render_settings_.padding) / calc_pos_idx();
	for (Stop* stop : stops_) {
		stop->svg_pos.x = stop->pos_idx * x_step + render_settings_.padding;
	}

	std::sort(std::begin(stops_), std::end(stops_),
		[](const Stop* lhs, const Stop* rhs)
		{ return lhs->map_pos.latitude < rhs->map_pos.latitude; });
	double y_step = (render_settings_.height - 2 * render_settings_.padding) / calc_pos_idx();
	double y_offset = render_settings_.height - render_settings_.padding;
	for (Stop* stop : stops_) {
		stop->svg_pos.y = y_offset - stop->pos_idx * y_step;
	}
}

void TransportMap::RenderSettings::Init(const Json::Dict& render_settings) {
	width = render_settings.at("width").AsDouble();
	height = render_settings.at("height").AsDouble();
	padding = render_settings.at("padding").AsDouble();
	stop_radius = render_settings.at("stop_radius").AsDouble();
	line_width = render_settings.at("line_width").AsDouble();

	if (auto it = render_settings.find("outer_margin"); it != render_settings.end())
		outer_margin = it->second.AsDouble();

	enum {
		LABEL_OFFSET_X,
		LABEL_OFFSET_Y
	};

	const Json::Array& bus_offset = render_settings.at("bus_label_offset").AsArray();
	bus_label = RenderSettings::Label{
		.font_size = render_settings.at("bus_label_font_size").AsInt(),
		.offset_x = bus_offset.at(LABEL_OFFSET_X).AsDouble(),
		.offset_y = bus_offset.at(LABEL_OFFSET_Y).AsDouble(),
	};
	const Json::Array& stop_offset = render_settings.at("stop_label_offset").AsArray();
	stop_label = RenderSettings::Label{
		.font_size = render_settings.at("stop_label_font_size").AsInt(),
		.offset_x = stop_offset.at(LABEL_OFFSET_X).AsDouble(),
		.offset_y = stop_offset.at(LABEL_OFFSET_Y).AsDouble(),
	};

	underlayer = RenderSettings::Underlayer{
		.color = AsColor(render_settings.at("underlayer_color")),
		.width = render_settings.at("underlayer_width").AsDouble()
	};

	if (auto it = render_settings.find("layers"); it != render_settings.end()) {
		for (const Json::Node& node : it->second.AsArray()) {
			auto to_layer_type = [](const string& type) {
				if (type == "bus_lines") return RenderSettings::LT_BUS_LINES;
				if (type == "bus_labels") return RenderSettings::LT_BUS_LABELS;
				if (type == "stop_points") return RenderSettings::LT_STOP_POINTS;
				if (type == "stop_labels") return RenderSettings::LT_STOP_LABELS;
				throw invalid_argument("invalid layer type - "s + type);
			};
			layers.push_back(to_layer_type(node.AsString()));
		}
	}

	const Json::Array& json_palette = render_settings.at("color_palette").AsArray();
	for (const auto& node : json_palette) {
		palette.push_back(AsColor(node));
	}
}

void TransportMap::ProcStops() {
	Svg::Text label_top = Svg::Text{}
		.SetFillColor("black")
		.SetFontFamily("Verdana")
		.SetFontSize(render_settings_.stop_label.font_size)
		.SetOffset({
			render_settings_.stop_label.offset_x,
			render_settings_.stop_label.offset_y
		});
	Svg::Text label_bot = Svg::Text{ label_top }
		.SetFillColor(render_settings_.underlayer.color)
		.SetStrokeColor(render_settings_.underlayer.color)
		.SetStrokeWidth(render_settings_.underlayer.width)
		.SetStrokeLineCap("round")
		.SetStrokeLineJoin("round");

	for (Stop* stop : stops_) {
		stop->circle
			.SetRadius(render_settings_.stop_radius)
			.SetFillColor("white")
			.SetCenter(stop->svg_pos);

		stop->label_top = label_top
			.SetData(stop->desc->name)
			.SetPoint(stop->svg_pos);

		stop->label_bot = label_bot
			.SetData(stop->desc->name)
			.SetPoint(stop->svg_pos);
	}
}

void TransportMap::ProcBuses() {
	Svg::Text label_top = Svg::Text{}
		.SetFillColor("black")
		.SetFontFamily("Verdana")
		.SetFontWeight("bold")
		.SetFontSize(render_settings_.bus_label.font_size)
		.SetOffset({
			render_settings_.bus_label.offset_x,
			render_settings_.bus_label.offset_y
		});
	Svg::Text label_bot = Svg::Text{ label_top }
		.SetFillColor(render_settings_.underlayer.color)
		.SetStrokeColor(render_settings_.underlayer.color)
		.SetStrokeWidth(render_settings_.underlayer.width)
		.SetStrokeLineCap("round")
		.SetStrokeLineJoin("round");

	unsigned int color_idx = 0;
	for (Bus* bus : buses_) {
		bus->color = render_settings_.palette[color_idx];
		color_idx = (color_idx + 1) % render_settings_.palette.size();

		bus->line.SetStrokeColor(bus->color)
			.SetStrokeWidth(render_settings_.line_width)
			.SetStrokeLineCap("round")
			.SetStrokeLineJoin("round");
		for (const Descriptions::Stop* stop : bus->desc->stops) {
			bus->line.AddPoint(stop_items_[stop->stop_id].svg_pos);
		}

		bus->label_bot = label_bot
			.SetData(bus->desc->name)
			.SetPoint(bus->first_stop->svg_pos);
		bus->label_top = label_top
			.SetFillColor(bus->color)
			.SetData(bus->desc->name)
			.SetPoint(bus->first_stop->svg_pos);
	}
}

std::string TransportMap::Render(const TransportRouter::RouteInfo* route) const {
	ostringstream oss;
	oss << Svg::Header;

	for (RenderSettings::LayerType layer_type : render_settings_.layers) {
		switch (layer_type) {
		case RenderSettings::LT_BUS_LINES:
			for (const Bus* bus : buses_) {
				bus->line.Render(oss);
			}
			break;
		case RenderSettings::LT_BUS_LABELS:
			for (const Bus* bus : buses_) {
				bus->label_bot.Render(oss);
				bus->label_top.Render(oss);
				if (bus->last_stop) {
					Svg::Text{ bus->label_bot }.SetPoint(bus->last_stop->svg_pos).Render(oss);
					Svg::Text{ bus->label_top }.SetPoint(bus->last_stop->svg_pos).Render(oss);
				}
			}
			break;
		case RenderSettings::LT_STOP_POINTS:
			for (const Stop* stop : stops_) {
				stop->circle.Render(oss);
			}
			break;
		case RenderSettings::LT_STOP_LABELS:
			for (const Stop* stop : stops_) {
				stop->label_bot.Render(oss);
				stop->label_top.Render(oss);
			}
			break;
		default:
			assert(!"this should never be called");
		}
	}

	if (route) {
		RenderRoute(oss, route);
	}

	oss << Svg::Ending;

	return oss.str();
}

void TransportMap::RenderRoute(ostream& os, const TransportRouter::RouteInfo* route) const {
	Svg::Rect{}
		.SetPoint(Svg::Point{-render_settings_.outer_margin, -render_settings_.outer_margin})
		.SetHeight(render_settings_.height + 2 * render_settings_.outer_margin)
		.SetWidth(render_settings_.width + 2 * render_settings_.outer_margin)
		.SetFillColor(render_settings_.underlayer.color)
		.Render(os);

	for (RenderSettings::LayerType layer_type : render_settings_.layers) {
		switch (layer_type) {
		case RenderSettings::LT_BUS_LINES:
			for (const auto& item : route->items) {
				if (const auto* p = get_if<TransportRouter::RouteInfo::BusItem>(&item)) {
					bus_items_[p->bus->bus_id].line.Render(
						os, p->first_stop_idx, p->span_count + 1);
				}
			}
			break;
		case RenderSettings::LT_BUS_LABELS:
			for (const auto& item : route->items) {
				if (const auto* p = get_if<TransportRouter::RouteInfo::BusItem>(&item)) {
					const Bus* bus = &bus_items_[p->bus->bus_id];
					std::array<const Stop*, 2> route_stops = {
						&stop_items_[bus->desc->stops[p->first_stop_idx]->stop_id],
						&stop_items_[bus->desc->stops[p->first_stop_idx + p->span_count]->stop_id]
					};
					for (const Stop* stop : route_stops) {
						if (stop == bus->first_stop) {
							bus->label_bot.Render(os);
							bus->label_top.Render(os);
						}
						else if (stop == bus->last_stop) {
							Svg::Text{ bus->label_bot }.SetPoint(bus->last_stop->svg_pos).Render(os);
							Svg::Text{ bus->label_top }.SetPoint(bus->last_stop->svg_pos).Render(os);
						}
					}
				}
			}
			break;
		case RenderSettings::LT_STOP_POINTS:
			for (const auto& item : route->items) {
				if (const auto* p = get_if<TransportRouter::RouteInfo::BusItem>(&item)) {
					const Bus* bus = &bus_items_[p->bus->bus_id];
					for (size_t i = 0; i <= p->span_count; ++i) {
						stop_items_[bus->desc->stops[p->first_stop_idx + i]->stop_id].circle.Render(os);
					}
				}
			}
			break;
		case RenderSettings::LT_STOP_LABELS: {
				const Stop* last_stop = nullptr;
				for (const auto& item : route->items) {
					if (const auto* p = get_if<TransportRouter::RouteInfo::BusItem>(&item)) {
						const Bus* bus = &bus_items_[p->bus->bus_id];
						const Stop* stop = &stop_items_[
							bus->desc->stops[p->first_stop_idx]->stop_id];
						last_stop = &stop_items_[
							bus->desc->stops[p->first_stop_idx + p->span_count]->stop_id];
						stop->label_bot.Render(os);
						stop->label_top.Render(os);
					}
				}
				last_stop->label_bot.Render(os);
				last_stop->label_top.Render(os);
			}
			break;
		default:
			assert(!"this should never be called");
		}
	}
}
