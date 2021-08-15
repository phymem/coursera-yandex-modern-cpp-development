#pragma once

#include "json.h"

#include <iostream>

class Descriptions;
class TransportMap;
class TransportRouter;

void ProcessRequests(
	std::ostream& os,
	const Descriptions& desc,
	const TransportRouter& router,
	const TransportMap& transport_map,
	const Json::Array& requests
);
