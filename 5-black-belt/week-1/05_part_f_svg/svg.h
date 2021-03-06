#pragma once

#include <iostream>
#include <variant>
#include <vector>

namespace Svg {

struct Point {
	double x;
	double y;
};

struct Rgb {
	unsigned char red;
	unsigned char green;
	unsigned char blue;

	friend std::ostream& operator << (std::ostream& os, const Rgb& rgb) {
		return os << "rgb("
			<< static_cast<int>(rgb.red) << ','
			<< static_cast<int>(rgb.green) << ','
			<< static_cast<int>(rgb.blue) << ')';
	}
};

struct Rgba {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	double alpha;

	friend std::ostream& operator << (std::ostream& os, const Rgba& rgba) {
		return os << "rgb("
			<< static_cast<int>(rgba.red) << ','
			<< static_cast<int>(rgba.green) << ','
			<< static_cast<int>(rgba.blue) << ','
			<< rgba.alpha << ')';
	}
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
const Color NoneColor{};

inline std::ostream& operator << (std::ostream& os, const Color& color) {
	struct color_visitor {
		color_visitor(std::ostream& os) : ros(os) {}
		std::ostream& ros;
		void operator () (std::monostate ) { ros << "none"; }
		void operator () (const std::string& str) { ros << str; }
		void operator () (Rgb rgb) { ros << rgb; }
		void operator () (Rgba rgba) { ros << rgba; }
	};
	std::visit(color_visitor(os), color);
	return os;
}

template <typename T>
class SvgObj {
public:

	typedef const SvgObj<T>& base_cref_t;

	SvgObj() :
		fill_color(NoneColor),
		stroke_color(NoneColor),
		stroke_width(1.0) {}

	T& SetFillColor(const Color& color) { fill_color = color; return static_cast<T&>(*this); }
	T& SetStrokeColor(const Color& color) { stroke_color = color; return static_cast<T&>(*this); }
	T& SetStrokeWidth(double width) { stroke_width = width; return static_cast<T&>(*this); }
	T& SetStrokeLineCap(const std::string& cap) { linecap = cap; return static_cast<T&>(*this); }
	T& SetStrokeLineJoin(const std::string& join) { linejoin = join; return static_cast<T&>(*this); }

	friend std::ostream& operator << (std::ostream& os, base_cref_t obj) {
		os << "fill=\"" << obj.fill_color << '"'
			<< " stroke=\"" << obj.stroke_color << '"'
			<< " stroke-width=\"" << obj.stroke_width << '"';
		if (!obj.linecap.empty())
			os << " stroke-linecap=\"" << obj.linecap << '"';
		if (!obj.linejoin.empty())
			os << " stroke-linejoin=\"" << obj.linejoin << '"';
		return os;
	}

private:

	Color fill_color;
	Color stroke_color;
	double stroke_width;
	std::string linecap;
	std::string linejoin;
};

class Circle : public SvgObj<Circle> {
public:

	Circle() :
		center{0, 0},
		radius(1.0) {}

	Circle& SetCenter(Point pnt) { center = pnt; return *this; }
	Circle& SetRadius(double rad) { radius = rad; return *this; }

	friend std::ostream& operator << (std::ostream& os, const Circle& circle) {
		return os << "<circle cx=\"" << circle.center.x
			<< "\" cy=\"" << circle.center.y
			<< "\" r=\"" << circle.radius
			<< "\" " << static_cast<Circle::base_cref_t>(circle) << " />";
	}

private:

	Point center;
	double radius;
};

class Text : public SvgObj<Text> {
public:

	Text() :
		point{0, 0},
		offset{0, 0},
		font_size(1) {}

	Text& SetPoint(Point pnt) { point = pnt; return *this; }
	Text& SetOffset(Point pnt) { offset = pnt; return *this; }
	Text& SetFontSize(uint32_t size) { font_size = size; return *this; }
	Text& SetFontFamily(const std::string& family) { font_family = family; return *this; }
	Text& SetData(const std::string& data) { text = data; return *this; }

	friend std::ostream& operator << (std::ostream& os, const Text& text) {
		os << "<text x=\"" << text.point.x
			<< "\" y=\"" << text.point.y
			<< "\" dx=\"" << text.offset.x
			<< "\" dy=\"" << text.offset.y
			<< "\" font-size=\"" << text.font_size << '"';
		if (!text.font_family.empty())
			os << " font-family=\"" << text.font_family << '"';
		return os << " " << static_cast<Text::base_cref_t>(text) << " >" << text.text << "</text>";
	}

private:

	Point point;
	Point offset;
	uint32_t font_size;
	std::string font_family;
	std::string text;
};

class Polyline : public SvgObj<Polyline> {
public:

	Polyline() {}

	Polyline& AddPoint(Point pnt) { points.push_back(pnt); return *this; }

	friend std::ostream& operator << (std::ostream& os, const Polyline& line) {
		os << "<polyline points=\"";
		for (unsigned int i = 0; i < line.points.size(); ++i) {
			if (i) os << ' ';
			const Point& pnt = line.points[i];
			os << pnt.x << ',' << pnt.y;
		}
		return os << "\" " << static_cast<Polyline::base_cref_t>(line) << " />";
	}

private:

	std::vector<Point> points;
};

class Document {
public:

	using ObjVariant = std::variant<Circle, Polyline, Text>;
	template <typename T>
	void Add(const T& obj) {
		objects.push_back( ObjVariant(obj) );
	}

	void Render(std::ostream& os) const {
		os << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
		"<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">";

		for (const ObjVariant& obj : objects) {
			std::visit([&os](const auto& p) { os << p; }, obj);
		}

		os << "</svg>";
	}

private:

	std::vector<ObjVariant> objects;
};

} // namespace Svg
