#include <iostream>
#include <map>
#include <set>
#include <vector>
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

inline bool operator < (const Rational& lhs, const Rational& rhs) {
	return lhs.Numerator() * rhs.Denominator() < rhs.Numerator() * lhs.Denominator();
}

int main() {
    {
        const set<Rational> rs = {{1, 2}, {1, 25}, {3, 4}, {3, 4}, {1, 2}};
        if (rs.size() != 3) {
            cout << "Wrong amount of items in the set" << endl;
            return 1;
        }

        vector<Rational> v;
        for (auto x : rs) {
            v.push_back(x);
        }
        if (v != vector<Rational>{{1, 25}, {1, 2}, {3, 4}}) {
            cout << "Rationals comparison works incorrectly" << endl;
            return 2;
        }
    }

    {
        map<Rational, int> count;
        ++count[{1, 2}];
        ++count[{1, 2}];

        ++count[{2, 3}];

        if (count.size() != 2) {
            cout << "Wrong amount of items in the map" << endl;
            return 3;
        }
    }

    cout << "OK" << endl;
    return 0;
}
