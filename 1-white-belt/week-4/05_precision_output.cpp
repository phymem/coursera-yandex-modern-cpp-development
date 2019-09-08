#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

int main() {

	ifstream fin("input.txt");

	if (fin.is_open()) {
			while (true) {
			double val;
			fin >> val;
			if (fin)
				cout << fixed << setprecision(3) << val << endl;
			else
				break;
		}
	}
	return 0;
}
