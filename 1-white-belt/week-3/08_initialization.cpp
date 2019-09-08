/*
class Incognizable {
public:
	Incognizable() : a(0), b(0) {}
	Incognizable(int _a) : a(_a), b(0) {}
	Incognizable(int _a, int _b) : a(_a), b(_b) {}

private:
	int a, b;
};
*/

struct Incognizable {
	int a;
	int b;
};

int main() {
	Incognizable a;
	Incognizable b = {};
	Incognizable c = {0};
	Incognizable d = {0, 1};
	return 0;
}
