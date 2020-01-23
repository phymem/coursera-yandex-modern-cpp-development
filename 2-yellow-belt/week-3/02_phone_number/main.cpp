#include <iostream>

#include "phone_number.h"

int main() {
	const char* nums[] = {
		"+7-495-111-22-33",
		"+7-495-1112233",
		"+323-22-460002",
		"+1-2-coursera-cpp",
		"1-2-333",
		"+7-1233"
	};

	for (unsigned int i = 0; i < sizeof(nums) / sizeof(*nums); ++i) {
		try {
			PhoneNumber pn(nums[i]);

			cout << pn.GetCountryCode() << ' '
				<< pn.GetCityCode() << ' '
				<< pn.GetLocalNumber() << ' '
				<< pn.GetInternationalNumber() << endl;
		}
		catch (const invalid_argument& e) {
			cout << nums[i] << " throws invalid_argument: " << e.what() << endl;
		}
	}

	return 0;
}
