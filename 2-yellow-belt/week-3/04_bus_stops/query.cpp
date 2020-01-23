#include "query.h"

istream& operator >> (istream& is, Query& q) {
	std::string type;
	is >> type;

	if (type == "NEW_BUS") {
		q.type = QueryType::NewBus;

		is >> q.bus;

		unsigned int num;
		is >> num;
		q.stops.resize(num);
		for (auto& stop : q.stops) {
			is >> stop;
		}
	}
	else if (type == "BUSES_FOR_STOP") {
		q.type = QueryType::BusesForStop;
		is >> q.stop;
	}
	else if (type == "STOPS_FOR_BUS") {
		q.type = QueryType::StopsForBus;
		is >> q.bus;
	}
	else if (type == "ALL_BUSES") {
		q.type = QueryType::AllBuses;
	}
	else
		throw runtime_error("unexpected query type");

	return is;
}
