#include "bus_manager.h"

void BusManager::AddBus(const string& bus, const vector<string>& vec) {
	buses_to_stops[bus] = vec;
	for (const auto& stop : vec) {
		stops_to_buses[stop].push_back(bus);
	}
}

BusesForStopResponse BusManager::GetBusesForStop(const string& stop) const {
	BusesForStopResponse ret;
	if (stops_to_buses.count(stop)) {
		for (const auto& bus : stops_to_buses.at(stop)) {
			ret.buses.push_back(bus);
		}
	}
	return ret;
}

StopsForBusResponse BusManager::GetStopsForBus(const string& bus) const {
	StopsForBusResponse ret;
	if (buses_to_stops.count(bus)) {
		for (const string& stop : buses_to_stops.at(bus)) {
			vector<string> buses;
			if (stops_to_buses.at(stop).size() == 1) {
				buses.push_back("no interchange");
			}
			else {
				for (const auto& other_bus : stops_to_buses.at(stop)) {
					if (bus != other_bus) {
						buses.push_back(other_bus);
					}
				}
			}
			ret.stops.push_back(make_pair(stop, buses));
		}
	}
	return ret;
}

AllBusesResponse BusManager::GetAllBuses() const {
	AllBusesResponse ret;
	for (const auto& it : buses_to_stops) {
		ret.buses.push_back(it);
	}
	return ret;
}

