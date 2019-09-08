#include <iostream>
using namespace std;

inline int gcd(int a, int b) {
	if (a > b) {
		int t = a;
		a = b;
		b = t;
	}
	while (int r = b % a) {
		b = a;
		a = r;
	}
	return a;
}

inline int sign(int a) {
	if (a > 0) return 1;
	if (a < 0) return -1;
	return 0;
}

class Rational {
public:
    Rational() : m_numerator(0), m_denominator(1) {}

    Rational(int numerator, int denominator) {
		if (!numerator) {
			m_numerator = 0;
			m_denominator = 1;
			return;
		}

		int n_sign = sign(numerator);
		int d_sign = sign(denominator);

		numerator = abs(numerator);
		denominator = abs(denominator);

		int div = gcd(numerator, denominator);
		m_numerator = numerator / div;
		m_denominator = denominator / div;

		if (n_sign != d_sign)
			m_numerator = -m_numerator;
    }

    int Numerator() const { return m_numerator; }
    int Denominator() const { return m_denominator; }

private:

	int m_numerator;
	int m_denominator;
};

int main() {
    {
        const Rational r(3, 10);
        if (r.Numerator() != 3 || r.Denominator() != 10) {
            cout << "Rational(3, 10) != 3/10" << endl;
            return 1;
        }
    }

    {
        const Rational r(8, 12);
        if (r.Numerator() != 2 || r.Denominator() != 3) {
            cout << "Rational(8, 12) != 2/3" << endl;
            return 2;
        }
    }

    {
        const Rational r(-4, 6);
        if (r.Numerator() != -2 || r.Denominator() != 3) {
            cout << "Rational(-4, 6) != -2/3" << endl;
            return 3;
        }
    }

    {
        const Rational r(4, -6);
        if (r.Numerator() != -2 || r.Denominator() != 3) {
            cout << "Rational(4, -6) != -2/3" << endl;
            return 3;
        }
    }

    {
        const Rational r(0, 15);
        if (r.Numerator() != 0 || r.Denominator() != 1) {
            cout << "Rational(0, 15) != 0/1" << endl;
            return 4;
        }
    }

    {
        const Rational defaultConstructed;
        if (defaultConstructed.Numerator() != 0 || defaultConstructed.Denominator() != 1) {
            cout << "Rational() != 0/1" << endl;
            return 5;
        }
    }

    cout << "OK" << endl;
    return 0;
}
