#include <iostream>
#include <sstream>
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

inline bool operator == (const Rational& lhs, const Rational& rhs) {
	return lhs.Numerator() == rhs.Numerator()
		&& lhs.Denominator() == rhs.Denominator();
}

inline Rational operator + (const Rational& lhs, const Rational& rhs) {
	return Rational(
		lhs.Numerator() * rhs.Denominator()
			+ rhs.Numerator() * lhs.Denominator(),
		lhs.Denominator() * rhs.Denominator());
}

inline Rational operator - (const Rational& lhs, const Rational& rhs) {
	return Rational(
		lhs.Numerator() * rhs.Denominator()
			- rhs.Numerator() * lhs.Denominator(),
		lhs.Denominator() * rhs.Denominator());
}

inline Rational operator * (const Rational& lhs, const Rational& rhs) {
	return Rational(
		lhs.Numerator() * rhs.Numerator(),
		lhs.Denominator() * rhs.Denominator());
}

inline Rational operator / (const Rational& lhs, const Rational& rhs) {
	return Rational(
		lhs.Numerator() * rhs.Denominator(),
		lhs.Denominator() * rhs.Numerator());
}

inline ostream& operator << (ostream& os, const Rational& r) {
	os << r.Numerator() << "/" << r.Denominator();
	return os;
}

inline istream& operator >> (istream& is, Rational& r) {
	int n, d;
	if (is >> n && is.ignore(1) && is >> d)
		r = Rational(n, d);
	return is;
}

int main() {
    {
        ostringstream output;
        output << Rational(-6, 8);
        if (output.str() != "-3/4") {
            cout << "Rational(-6, 8) should be written as \"-3/4\"" << endl;
            return 1;
        }
    }

    {
        istringstream input("5/7");
        Rational r;
        input >> r;
        bool equal = r == Rational(5, 7);
        if (!equal) {
            cout << "5/7 is incorrectly read as " << r << endl;
            return 2;
        }
    }

    {
        istringstream input("5/7 10/8");
        Rational r1, r2;
        input >> r1 >> r2;
        bool correct = r1 == Rational(5, 7) && r2 == Rational(5, 4);
        if (!correct) {
            cout << "Multiple values are read incorrectly: " << r1 << " " << r2 << endl;
            return 3;
        }

        input >> r1;
        input >> r2;
        correct = r1 == Rational(5, 7) && r2 == Rational(5, 4);
        if (!correct) {
            cout << "Read from empty stream shouldn't change arguments: " << r1 << " " << r2 << endl;
            return 4;
        }
    }

    cout << "OK" << endl;
    return 0;
}
