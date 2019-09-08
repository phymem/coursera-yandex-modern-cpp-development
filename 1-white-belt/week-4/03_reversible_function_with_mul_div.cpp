#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Function {
public:

	void AddPart(char op, double val) {
		m_parts.push_back(FuncPart(op, val));
	}
	void Invert() {
		for (FuncPart& p : m_parts) {
			p.Invert();
		}
		reverse(m_parts.begin(), m_parts.end());
	}
	double Apply(double value) const {
		for (const FuncPart& p : m_parts) {
			value = p.Apply(value);
		}
		return value;
	}

private:

	struct FuncPart {
		double	m_val;
		char	m_op;

		FuncPart(char op, double val) : m_op(op), m_val(val) {}

		void Invert() {
			switch (m_op) {
			case '-': m_op = '+'; break;
			case '+': m_op = '-'; break;
			case '*': m_op = '/'; break;
			case '/': m_op = '*'; break;
			}
		}

		double Apply(double val) const {
			switch (m_op) {
			case '-': return val - m_val;
			case '+': return val + m_val;
			case '*': return val * m_val;
			case '/': return val / m_val;
			}
			return 0;
		}
	};

	vector<FuncPart> m_parts;
};

#define DBG
#ifdef DBG
struct Image {
	double quality;
	double freshness;
	double rating;
};

struct Params {
	double a;
	double b;
	double c;
};

Function MakeWeightFunction(const Params& params, const Image& image) {
	Function function;
	function.AddPart('*', params.a);
	function.AddPart('-', image.freshness * params.b);
	function.AddPart('+', image.rating * params.c);
	return function;
}

double ComputeImageWeight(const Params& params, const Image& image) {
	Function function = MakeWeightFunction(params, image);
	return function.Apply(image.quality);
}

double ComputeQualityByWeight(const Params& params, const Image& image, double weight) {
	Function function = MakeWeightFunction(params, image);
	function.Invert();
	return function.Apply(weight);
}

int main() {
	Image image = {10, 2, 6};
	Params params = {4, 2, 6};
	cout << ComputeImageWeight(params, image) << endl;
	cout << ComputeQualityByWeight(params, image, 52) << endl;
	return 0;
}
#endif // DBG
