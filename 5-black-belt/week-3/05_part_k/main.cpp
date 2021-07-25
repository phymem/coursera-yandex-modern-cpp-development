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

	const Json::Dict* render_settings = nullptr;
	if (auto it = input_map.find("render_settings"); it != input_map.end())
		render_settings = &it->second.AsMap();

	Descriptions descriptions(
		input_map.at("base_requests").AsArray(),
		render_settings
	);

	TransportRouter router(
		descriptions,
		input_map.at("routing_settings").AsMap()
	);

	TransportMap transport_map(
		descriptions,
		render_settings);

	ProcessRequests(
		cout,
		descriptions,
		router,
		transport_map,
		input_map.at("stat_requests").AsArray()
	);

	cout << endl;

	return 0;
}
