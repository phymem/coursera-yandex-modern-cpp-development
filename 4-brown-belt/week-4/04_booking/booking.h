#pragma once

#include <utility>

namespace RAII {

template <typename Provider>
class Booking {
public:

	Booking() :
		provider(nullptr),
		booking_id(0) {}

	Booking(Booking&& booking) :
		provider(booking.provider),
		booking_id(booking.booking_id)
	{ booking.provider = nullptr; }

	Booking(Provider* provider_, int booking_id_) :
		provider(provider_),
		booking_id(booking_id_) {}

	Booking& operator = (Booking&& booking) {
		std::swap(provider, booking.provider);
		std::swap(booking_id, booking.booking_id);
		return *this;
	}

	~Booking() {
		if (provider) {
			provider->CancelOrComplete(*this);
		}
	}

	Booking(const Booking& booking) = delete;
	Booking& operator = (const Booking&) = delete;

private:

	Provider* provider;
	typename Provider::BookingId booking_id;
};

} // namespace RAII
