#include "json.h"
#include "requests.h"
#include "descriptions.h"
#include "transport_map.h"
#include "transport_router.h"

#include <iostream>

using namespace std;

int main() {
	const auto input_doc = Json::Load(cin);
	const auto& input_map = input_doc.GetRoot().AsMap();

	Descriptions descriptions(
		input_map.at("base_requests").AsArray()
	);

	TransportRouter router(
		descriptions,
		input_map.at("routing_settings").AsMap()
	);

	TransportMap transport_map(
		descriptions,
		input_map.at("render_settings").AsMap()
	);

	PrintValue(
		ProcessRequests(
			descriptions,
			router,
			transport_map,
			input_map.at("stat_requests").AsArray()
		),
		cout
	);

	cout << endl;

	return 0;
}
