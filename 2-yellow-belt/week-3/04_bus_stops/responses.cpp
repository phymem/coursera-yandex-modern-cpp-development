#include "responses.h"

ostream& operator << (ostream& os, const BusesForStopResponse& r) {
	if (r.buses.empty()) {
		os << "No stop";
	}
	else {
		for (const auto& s : r.buses) {
			os << s << " ";
		}
	}
	return os;
}

ostream& operator << (ostream& os, const StopsForBusResponse& r) {
	if (r.stops.empty()) {
		os << "No bus";
	}
	else {
		const char* delim = "";
		for (const auto& p : r.stops) {
			os << delim << "Stop " << p.first << ": ";
			for (const auto& s : p.second) {
				os << s << " ";
			}
			delim = "\n";
		}
	}
	return os;
}

ostream& operator << (ostream& os, const AllBusesResponse& r) {
	if (r.buses.empty()) {
		os << "No buses";
	}
	else {
		const char* delim = "";
		for (const auto& p : r.buses) {
			os << delim << "Bus " << p.first << ": ";
			for (const auto& s : p.second) {
				os << s << " ";
			}
			delim = "\n";
		}
	}
	return os;
}
