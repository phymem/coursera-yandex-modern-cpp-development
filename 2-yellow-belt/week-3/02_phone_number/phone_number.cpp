#include <stdexcept>

#include "phone_number.h"

PhoneNumber::PhoneNumber(const string& number) {
	if (number.empty() || number[0] != '+')
		throw invalid_argument("+ sign missing");

	unsigned int ndx = 1; // skip '+'
	while (ndx < number.length()) {
		char ch = number[ndx++];
		if (ch == '-')
			break;
		country_code_ += ch;
	}
	if (country_code_.empty())
		throw invalid_argument("empty country code");

	while (ndx < number.length()) {
		char ch = number[ndx++];
		if (ch == '-')
			break;
		city_code_ += ch;
	}
	if (city_code_.empty())
		throw invalid_argument("empty city code");

	local_number_ = number.substr(ndx, number.length());
	if (local_number_.empty())
		throw invalid_argument("empty local number");
}

string PhoneNumber::GetCountryCode() const {
	return country_code_;
}

string PhoneNumber::GetCityCode() const {
	return city_code_;
}

string PhoneNumber::GetLocalNumber() const {
	return local_number_;
}
string PhoneNumber::GetInternationalNumber() const {
	return "+" + country_code_
		+ '-' + city_code_
		+ '-' + local_number_;
}
