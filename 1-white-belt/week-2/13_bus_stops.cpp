#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

int main() {

	int n;
	cin >> n;

	map<string, vector<string>> bus_to_stop;
	map<string, vector<string>> stop_to_bus;

	while (n--) {
		string cmd;
		cin >> cmd;

		if (cmd == "NEW_BUS") {
			string bus;
			cin >> bus;

			int num;
			cin >> num;

			while (num--) {
				string stop;
				cin >> stop;

				bus_to_stop[bus].push_back(stop);
				stop_to_bus[stop].push_back(bus);
			}
		}
		else if (cmd == "BUSES_FOR_STOP") {
			string stop;
			cin >> stop;

			auto it = stop_to_bus.find(stop);
			if (it == stop_to_bus.end()) {
				cout << "No stop\n";
			}
			else {
				const char* delim = "";
				for (auto bus : it->second) {
					cout << delim << bus;
					delim = " ";
				}
				cout << '\n';
			}
		}
		else if (cmd == "STOPS_FOR_BUS") {
			string bus;
			cin >> bus;

			auto it_b2s = bus_to_stop.find(bus);
			if (it_b2s == bus_to_stop.end()) {
				cout << "No bus\n";
			}
			else {
				for (const auto& dd : it_b2s->second) {
					cout << "Stop " << dd << ":";
					auto it_s2b = stop_to_bus.find(dd);
					if (it_s2b == stop_to_bus.end() || it_s2b->second.size() == 1) {
						cout << " no interchange\n";
					}
					else {
						for (auto bb : it_s2b->second) {
							if (bb != bus)
								cout << " " << bb;
						}
						cout << '\n';
					}
				}
			}
		}
		else if (cmd == "ALL_BUSES") {
			if (bus_to_stop.empty()) {
				cout << "No buses\n";
			}
			else {
				for (const auto& pair : bus_to_stop) {
					cout << "Bus " << pair.first << ":";
					for (const auto& stop : pair.second) {
						cout << " " << stop;
					}
					cout << '\n';
				}
			}
		}
	}

	return 0;
}
