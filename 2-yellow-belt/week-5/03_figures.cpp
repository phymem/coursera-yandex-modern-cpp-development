#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <memory>
#include <vector>
#include <cmath>

using namespace std;

class Figure {
public:
	virtual const string& Name() const = 0;
	virtual float Perimeter() const = 0;
	virtual float Area() const = 0;
};

class Triangle : public Figure {
public:

	Triangle(float a, float b, float c) :
		name_("TRIANGLE"), a_(a), b_(b), c_(c) {}

	const string& Name() const override {
		return name_;
	}
	float Perimeter() const override {
		return a_ + b_ + c_;
	}
	float Area() const override {
		float p = (a_ + b_ + c_) / 2.0;
		return sqrt(p * (p - a_) * (p - b_) * (p - c_));
	}

private:
	const string name_;
	float a_, b_, c_;
};

class Rect : public Figure {
public:

	Rect(float w, float h) :
		name_("RECT"), w_(w), h_(h) {}

	const string& Name() const override {
		return name_;
	}
	float Perimeter() const override {
		return 2.0 * (w_ + h_);
	}
	float Area() const override {
		return w_ * h_;
	}

private:
	const string name_;
	float w_, h_;
};

class Circle : public Figure {
public:

	Circle(float r) :
		name_("CIRCLE"), r_(r) {}

	const string& Name() const override {
		return name_;
	}
	float Perimeter() const override {
		return 2 * 3.14 * r_;
	}
	float Area() const override {
		return 3.14 * r_ * r_;
	}

private:
	const string name_;
	float r_;
};

shared_ptr<Figure> CreateFigure(istream& is)
{
	string type;
	is >> type;

	if (type == "TRIANGLE") {
		int a, b, c;
		is >> a >> b >> c;
		return make_shared<Triangle>(a, b, c);
	}
	else if (type == "RECT") {
		int w, h;
		is >> w >> h;
		return make_shared<Rect>(w, h);
	}

	assert(type == "CIRCLE");

	int r;
	is >> r;
	return make_shared<Circle>(r);
}

int main() {
	vector<shared_ptr<Figure>> figures;
	for (string line; getline(cin, line); ) {
		istringstream is(line);

		string command;
		is >> command;
		if (command == "ADD") {
			figures.push_back(CreateFigure(is));
		} else if (command == "PRINT") {
			for (const auto& current_figure : figures) {
				cout << fixed << setprecision(3)
					<< current_figure->Name() << " "
					<< current_figure->Perimeter() << " "
					<< current_figure->Area() << endl;
			}
		}
	}
	return 0;
}
