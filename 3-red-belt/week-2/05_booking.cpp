#include <iostream>
#include <iomanip>
#include <string>
#include <list>
#include <set>
#include <map>

using namespace std;

class BookingManager {
public:

	void Book(
		long long int f_time,
		const string& f_hotel_name,
		unsigned int f_client_id,
		unsigned int f_rooms) {

		m_bookings.push_back({ f_time, f_hotel_name, f_client_id, f_rooms });

		HotelStat& stat = m_hotels[f_hotel_name];
		stat.m_clients[f_client_id] += f_rooms;
		stat.m_rooms_total += f_rooms;

		while ((f_time - m_bookings.front().m_time) >= MAX_TIME_RANGE) {
			const auto& b = m_bookings.front();

			auto it_hotel = m_hotels.find(b.m_hotel_name);
			it_hotel->second.m_rooms_total -= b.m_rooms;

			if (!it_hotel->second.m_rooms_total) {
				m_hotels.erase(it_hotel);
			}
			else {
				auto it_client = it_hotel->second.m_clients.find(b.m_client_id);
				it_client->second -= b.m_rooms;

				if (!it_client->second)
					it_hotel->second.m_clients.erase(it_client);
			}

			m_bookings.pop_front();
		}
	}

	unsigned int Clients(const string& f_hotel_name) const {
		const auto it = m_hotels.find(f_hotel_name);
		if (it == end(m_hotels))
			return 0;
		return it->second.m_clients.size();
	}

	unsigned int Rooms(const string& f_hotel_name) const {
		const auto it = m_hotels.find(f_hotel_name);
		if (it == end(m_hotels))
			return 0;
		return it->second.m_rooms_total;
	}

private:

	const long long int MAX_TIME_RANGE = 86400;

	struct Booking {
		long long int m_time;
		string m_hotel_name;
		unsigned int m_client_id;
		unsigned int m_rooms;

		Booking(
			long long int f_time,
			string f_hotel_name,
			unsigned int f_client_id,
			unsigned int f_rooms) :
			m_time(f_time),
			m_hotel_name(f_hotel_name),
			m_client_id(f_client_id),
			m_rooms(f_rooms) {}
	};

	struct HotelStat {
		map<int, int> m_clients;
		unsigned int m_rooms_total;
	};

	list<Booking> m_bookings;
	map<string, HotelStat> m_hotels;
};

int main() {

	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	BookingManager booking;

	unsigned int query_count;
	cin >> query_count;

	while (query_count--) {
		string query_type;
		cin >> query_type;

		if (query_type == "BOOK") {
			unsigned int client_id;
			unsigned int rooms;
			long long int time;
			string hotel_name;

			cin >> time >> hotel_name >> client_id >> rooms;
			booking.Book(time, hotel_name, client_id, rooms);
		}
		else if (query_type == "CLIENTS") {
			string hotel_name;
			cin >> hotel_name;
			cout << booking.Clients(hotel_name) << '\n';
		}
		else if (query_type == "ROOMS") {
			string hotel_name;
			cin >> hotel_name;
			cout << booking.Rooms(hotel_name) << '\n';
		}
	}

	return 0;
}
