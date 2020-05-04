#include <unordered_map>
#include <iostream>
#include <list>

using namespace std;

struct Booking {
	string m_hotel;
	long long m_time;
	unsigned int m_client;
	unsigned int m_rooms;
};

istream& operator >> (istream& is, Booking& booking) {
	return is >> booking.m_time
		>> booking.m_hotel
		>> booking.m_client
		>> booking.m_rooms;
}

const long long DAY_SEC = 86400;

class BookingManager {
public:

	void Book(const Booking& booking) {

		auto it_b = m_bookings.begin();
		while (it_b != m_bookings.end() && it_b->m_time + DAY_SEC <= booking.m_time) {

			auto it_s = m_stat.find(it_b->m_hotel);
			it_s->second.m_rooms -= it_b->m_rooms;
			if (!it_s->second.m_rooms) {
				m_stat.erase(it_s);
			}
			else {
				auto it_c = it_s->second.m_clients.find(it_b->m_client);
				it_c->second--;
				if (!it_c->second) {
					it_s->second.m_clients.erase(it_c);
				}
			}

			it_b = m_bookings.erase(it_b);
		}

		m_bookings.push_back(booking);
		Stat& stat = m_stat[booking.m_hotel];
		stat.m_rooms += booking.m_rooms;
		stat.m_clients[booking.m_client]++;
	}

	unsigned int Rooms(const string& hotel) {
		if (auto it_s = m_stat.find(hotel); it_s != m_stat.end())
			return it_s->second.m_rooms;
		return 0;
	}

	unsigned int Clients(const string& hotel) {
		if (auto it_s = m_stat.find(hotel); it_s != m_stat.end())
			return it_s->second.m_clients.size();
		return 0;
	}

private:

	struct Stat {
		unsigned int m_rooms;
		unordered_map<unsigned int, unsigned int> m_clients;
	};

	list<Booking> m_bookings;
	unordered_map<string, Stat> m_stat;
};

int main() {

	BookingManager booking_manager;

	int num;
	cin >> num;

	while (num--) {
		string command;
		cin >> command;

		if (command == "BOOK") {
			Booking booking;
			cin >> booking;

			booking_manager.Book(booking);
		}
		else if (command == "ROOMS") {
			string hotel;
			cin >> hotel;

			cout << booking_manager.Rooms(hotel) << "\n";
		}
		else if (command == "CLIENTS") {
			string hotel;
			cin >> hotel;

			cout << booking_manager.Clients(hotel) << "\n";
		}
	}

	return 0;
}
