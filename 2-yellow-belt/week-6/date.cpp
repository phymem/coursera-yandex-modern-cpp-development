#include "date.h"

Date ParseDate(istream& is)
{
	int yy, mm, dd;

	if (!(is >> yy) || is.peek() != '-' || !is.ignore(1)
		|| !(is >> mm) || is.peek() != '-' || !is.ignore(1)
		|| !(is >> dd))
		throw logic_error("Wrong date format");

	if (mm < 1 || mm > 12)
		throw logic_error(string("Month value is invalid: ") + to_string(mm));

	if (dd < 1 || dd > 31)
		throw logic_error(string("Day value is invalid: ") + to_string(dd));

	return Date(yy, mm, dd);
}
