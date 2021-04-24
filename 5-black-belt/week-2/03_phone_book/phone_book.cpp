#include <algorithm>

#include "phone_book.h"
#include "contact.pb.h"

PhoneBook::PhoneBook(ContactVec contacts) :
	contacts_(std::move(contacts))
{
	std::sort(std::begin(contacts_), std::end(contacts_),
		[](const Contact& lhs, const Contact& rhs) {
			return std::lexicographical_compare(
				std::begin(lhs.name), std::end(lhs.name),
				std::begin(rhs.name), std::end(rhs.name));
		}
	);
}

PhoneBook::ContactRange PhoneBook::FindByNamePrefix(std::string_view name_prefix) const {
	if (name_prefix.empty()) {
		return ContactRange(
			std::begin(contacts_),
			std::end(contacts_)
		);
	}
	struct prefix_comp {
		bool operator() (const Contact& cont, std::string_view pref) {
			return cont.name.substr(0, pref.length()) < pref;
		}
		bool operator() (std::string_view pref, const Contact& cont) {
			return pref < cont.name.substr(0, pref.length());
		}
	};
	auto result = std::equal_range(
		std::begin(contacts_),
		std::end(contacts_),
		name_prefix,
		prefix_comp {}
	);
	return ContactRange(result.first, result.second);
}

void PhoneBook::SaveTo(std::ostream& output) const {
	PhoneBookSerialize::ContactList list;
	for (const Contact& cc : contacts_) {
		PhoneBookSerialize::Contact* pp = list.add_contact();
		*pp->mutable_name() = cc.name;
		if (cc.birthday) {
			PhoneBookSerialize::Date* dd = pp->mutable_birthday();
			dd->set_year(cc.birthday->year);
			dd->set_month(cc.birthday->month);
			dd->set_day(cc.birthday->day);
		}
		for (const std::string& number : cc.phones) {
			*pp->add_phone_number() = number;
		}
	}
	list.SerializeToOstream(&output);
}

PhoneBook DeserializePhoneBook(std::istream& input) {
	PhoneBookSerialize::ContactList list;
	list.ParseFromIstream(&input);

	PhoneBook::ContactVec contacts;
	for (int i = 0; i < list.contact_size(); ++i) {
		const PhoneBookSerialize::Contact& pp = list.contact(i);
		Contact cc;
		cc.name = pp.name();
		if (pp.has_birthday()) {
			const PhoneBookSerialize::Date& dd = pp.birthday();
			cc.birthday = std::make_optional(
				Date{dd.year(), dd.month(), dd.day()});
		}
		for (int i = 0; i < pp.phone_number_size(); ++i) {
			cc.phones.push_back(pp.phone_number(i));
		}
		contacts.push_back(cc);
	}

	return PhoneBook(std::move(contacts));
}
