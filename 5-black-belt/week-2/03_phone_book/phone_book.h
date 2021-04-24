#pragma once

#include "iterator_range.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <iosfwd>

struct Date {
	int year, month, day;
};

struct Contact {
	std::string name;
	std::optional<Date> birthday;
	std::vector<std::string> phones;
};

class PhoneBook {
public:

	using ContactVec = std::vector<Contact>;
	using ContactRange = IteratorRange<ContactVec::const_iterator>;

	explicit PhoneBook(ContactVec contacts);

	ContactRange FindByNamePrefix(std::string_view name_prefix) const;

	void SaveTo(std::ostream& output) const;

private:

	ContactVec contacts_;
};

PhoneBook DeserializePhoneBook(std::istream& input);
