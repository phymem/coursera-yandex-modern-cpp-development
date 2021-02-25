#include "descriptions.h"
#include "json.h"
#include "requests.h"
#include "sphere.h"
#include "transport_catalog.h"
#include "utils.h"

#include <iostream>

using namespace std;

int main() {
	const auto input_doc = Json::Load(cin);
	const auto& input_map = input_doc.GetRoot().AsMap();

	const Json::Dict* render_settings = nullptr;
	if (auto it = input_map.find("render_settings"); it != input_map.end())
		render_settings = &it->second.AsMap();

	const TransportCatalog db(
		Descriptions::ReadDescriptions(input_map.at("base_requests").AsArray()),
		input_map.at("routing_settings").AsMap(),
		render_settings
	);

	Json::PrintValue(
		Requests::ProcessAll(db, input_map.at("stat_requests").AsArray()),
		cout
	);
	cout << endl;

	return 0;
}
