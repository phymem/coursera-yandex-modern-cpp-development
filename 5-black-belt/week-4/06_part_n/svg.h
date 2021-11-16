#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace Svg {

inline static const std::string Header = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">";
inline static const std::string Ending = "</svg>";

struct Point {
	double x = 0;
	double y = 0;
};

struct Rgb {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

struct Rgba : Rgb {
	double opacity;
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
const Color NoneColor{};

void RenderColor(std::ostream& out, const Color& color);

class Object {
public:
	virtual void Render(std::ostream& out) const = 0;
	virtual ~Object() = default;
};

template <typename Owner>
class PathProps {
public:
	Owner& SetFillColor(const Color& color);
	Owner& SetStrokeColor(const Color& color);
	Owner& SetStrokeWidth(double value);
	Owner& SetStrokeLineCap(const std::string& value);
	Owner& SetStrokeLineJoin(const std::string& value);
	void RenderAttrs(std::ostream& out) const;

protected:
	Color fill_color_;
	Color stroke_color_;
	double stroke_width_ = 1.0;
	std::optional<std::string> stroke_line_cap_;
	std::optional<std::string> stroke_line_join_;

private:
	Owner& AsOwner();
};

class Circle : public Object, public PathProps<Circle> {
public:
	Circle& SetCenter(Point point);
	Circle& SetRadius(double radius);
	void Render(std::ostream& out) const override;

private:
	Point center_;
	double radius_ = 1;
};

class Polyline : public Object, public PathProps<Polyline> {
public:
	Polyline& AddPoint(Point point);

	void Render(std::ostream& out) const override {
		Render(out, 0, points_.size());
	}

	void Render(std::ostream& out, size_t from, size_t count) const {
		out << "<polyline points=\"";
		for (unsigned int i = 0; i < count; ++i) {
			const Point& pnt = points_[i + from];
			out << pnt.x << "," << pnt.y << " ";
		}
		out << "\" ";
		PathProps::RenderAttrs(out);
		out << "/>";
	}

private:
	std::vector<Point> points_;
};

class Rect : public Object, public PathProps<Polyline> {
public:

	Rect& SetPoint(Point pnt) { point_ = pnt; return *this; }
	Rect& SetWidth(double w) { width_ = w; return *this; }
	Rect& SetHeight(double h) { height_ = h; return *this; }

	void Render(std::ostream& out) const override {
		out << "<rect ";
		out << "x=\"" << point_.x << "\" ";
		out << "y=\"" << point_.y << "\" ";
		out << "width=\"" << width_ << "\" ";
		out << "height=\"" << height_ << "\" ";
		PathProps::RenderAttrs(out);
		out << "/>";
	}

private:

	Point point_;
	double width_;
	double height_;
};

class Text : public Object, public PathProps<Text> {
public:
	Text& SetPoint(Point point);
	Text& SetOffset(Point point);
	Text& SetFontSize(uint32_t size);
	Text& SetFontFamily(const std::string& value);
	Text& SetFontWeight(const std::string& value);
	Text& SetData(const std::string& data);
	void Render(std::ostream& out) const override;

private:
	Point point_;
	Point offset_;
	uint32_t font_size_ = 1;
	std::optional<std::string> font_family_;
	std::optional<std::string> font_weight_;
	std::string data_;
};

class Document : public Object {
public:
	template <typename ObjectType>
	void Add(ObjectType object);

	void Render(std::ostream& out) const override;

private:
	std::vector<std::unique_ptr<Object>> objects_;
};


template <typename Owner>
Owner& PathProps<Owner>::AsOwner() {
	return static_cast<Owner&>(*this);
}

template <typename Owner>
Owner& PathProps<Owner>::SetFillColor(const Color& color) {
	fill_color_ = color;
	return AsOwner();
}

template <typename Owner>
Owner& PathProps<Owner>::SetStrokeColor(const Color& color) {
	stroke_color_ = color;
	return AsOwner();
}

template <typename Owner>
Owner& PathProps<Owner>::SetStrokeWidth(double value) {
	stroke_width_ = value;
	return AsOwner();
}

template <typename Owner>
Owner& PathProps<Owner>::SetStrokeLineCap(const std::string& value) {
	stroke_line_cap_ = value;
	return AsOwner();
}

template <typename Owner>
Owner& PathProps<Owner>::SetStrokeLineJoin(const std::string& value) {
	stroke_line_join_ = value;
	return AsOwner();
}

template <typename Owner>
void PathProps<Owner>::RenderAttrs(std::ostream& out) const {
	out << "fill=\"";
	RenderColor(out, fill_color_);
	out << "\" ";
	out << "stroke=\"";
	RenderColor(out, stroke_color_);
	out << "\" ";
	out << "stroke-width=\"" << stroke_width_ << "\" ";
	if (stroke_line_cap_) {
		out << "stroke-linecap=\"" << *stroke_line_cap_ << "\" ";
	}
	if (stroke_line_join_) {
		out << "stroke-linejoin=\"" << *stroke_line_join_ << "\" ";
	}
}

template <typename ObjectType>
void Document::Add(ObjectType object) {
	objects_.push_back(std::make_unique<ObjectType>(std::move(object)));
}

} // namespace Svg
