#include "stats.h"

HttpRequest ParseRequest(string_view line) {
	size_t method_first = 0;
	for (; method_first < line.size() && isspace(line[method_first]); ++method_first);

	size_t method_last = method_first;
	for (; method_last < line.size() && isalpha(line[method_last]); ++method_last);

	size_t uri_first = method_last;
	for (; uri_first < line.size() && isspace(line[uri_first]); ++uri_first);

	size_t uri_last = uri_first;
	for (; uri_last < line.size() && !isspace(line[uri_last]); ++uri_last);

	size_t protocol_first = uri_last;
	for (; protocol_first < line.size() && isspace(line[protocol_first]); ++protocol_first);

	return {
		{ line.data() + method_first, method_last - method_first },
		{ line.data() + uri_first, uri_last - uri_first },
		{ line.data() + protocol_first, line.size() - protocol_first },
	};
}
