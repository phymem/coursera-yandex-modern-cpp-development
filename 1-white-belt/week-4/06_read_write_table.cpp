#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;

int main() {

	ifstream fin("input.txt");

	int n, m;
	fin >> n >> m;

	for (int i = 0; i < n; ++i) {
		if (i) cout << endl;
		for (int j = 0; j < m; ++j) {

			if (j) {
				fin.ignore(1);
				cout << ' ';
			}

			int v;
			fin >> v;

			cout << setw(10) << v;
		}
	}
	return 0;
}
