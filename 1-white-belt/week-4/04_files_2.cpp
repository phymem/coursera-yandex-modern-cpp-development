#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {

	ifstream fin("input.txt");
	ofstream fout("output.txt");

	string line;
	while (getline(fin, line)) {
		fout << line << endl;
	}

	return 0;
}
