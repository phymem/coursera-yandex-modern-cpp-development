#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

struct CityData {
	const char* m_name;
	double m_latitude;
	double m_longitude;
};

vector<CityData> input_data {
#include "make_input_data.h"
};

int main() {
	const unsigned int MAX_CITIES = 2000;
	const unsigned int MAX_BUSSES = 2000;
	const unsigned int MAX_REQS = 2000;
	const unsigned int BUS_OFFSET = 1000;

	cout << fixed << setprecision(16);

	cout << (MAX_CITIES + MAX_BUSSES) << "\n";
	for (unsigned int i = 0; i < MAX_CITIES; ++i) {
		CityData* d = &input_data[i];
		cout << "Stop " << d->m_name << ": "
			<< d->m_latitude << ", "
			<< d->m_longitude << "\n";
	}

	for (unsigned int i = 0; i < MAX_BUSSES; ++i) {
		const char* delim = (rand() % 2) ? " - " : " > ";
		unsigned int num_stops = 30 + (rand() % 70);

		cout << "Bus " << (BUS_OFFSET + i) << ": ";
		for (unsigned int j = 0; j < num_stops; ++j) {
			CityData* d = &input_data[rand() % MAX_CITIES];
			cout << (j ? delim : "") << d->m_name;
		}
		cout << "\n";
	}

	cout << (MAX_REQS + MAX_REQS) << "\n";
	for (unsigned int i = 0; i < MAX_REQS; ++i) {
		cout << "Bus " << (BUS_OFFSET + rand() % MAX_BUSSES) << "\n";
	}
	for (unsigned int i = 0; i < MAX_REQS; ++i) {
		cout << "Stop " << input_data[rand() % input_data.size()].m_name << "\n";
	}

	return 0;
}
