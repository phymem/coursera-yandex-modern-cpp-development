#include "Common.h"

using namespace std;

class ShapeImp : public IShape {
public:

	ShapeImp(ShapeType type) :
		shape_type(type) {}

	explicit ShapeImp(const ShapeImp& shape) :
		texture(shape.texture),
		shape_type(shape.shape_type),
		position(shape.position),
		size(shape.size) {}

	unique_ptr<IShape> Clone() const override { return make_unique<ShapeImp>(*this); }

	void SetPosition(Point pnt) override { position = pnt; }
	Point GetPosition() const override { return position; }

	void SetSize(Size sz) override { size = sz; }
	Size GetSize() const override { return size; }

	void SetTexture(shared_ptr<ITexture> tex) override { texture = tex; }
	ITexture* GetTexture() const override { return texture.get(); }

	void Draw(Image& image) const override {

		if (image.empty())
			return;

		const Size draw_area_size = {
			std::min<int>(size.width, (int)image[0].length() - position.x),
			std::min<int>(size.height, (int)image.size() - position.y)
		};

		if (draw_area_size.width < 0 || draw_area_size.height < 0)
			return;

		auto is_point_in_rectangle = [](Point p, Size sz) {
			return p.x < sz.width && p.y < sz.height;
		};

		Point draw_point;
		for (draw_point.x = 0; draw_point.x < draw_area_size.width; ++draw_point.x) {
			for (draw_point.y = 0; draw_point.y < draw_area_size.height; ++draw_point.y) {
				if ((shape_type == ShapeType::Ellipse && IsPointInEllipse(draw_point, size))
					 || (shape_type == ShapeType::Rectangle && is_point_in_rectangle(draw_point, size))) {
					char color = '.';
					if (texture && is_point_in_rectangle(draw_point, texture->GetSize())) {
						color = texture->GetImage()[draw_point.y][draw_point.x];
					}
					image[position.y + draw_point.y][position.x + draw_point.x] = color;
				}
			}
		}
	}

private:

	shared_ptr<ITexture> texture;
	ShapeType shape_type;
	Point position;
	Size size;
};

// Напишите реализацию функции
unique_ptr<IShape> MakeShape(ShapeType shape_type) {
	return make_unique<ShapeImp>(shape_type);
}
