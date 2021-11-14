#pragma once

#include "json.h"

#include <iostream>

class Descriptions;
class TransportMap;
class TransportRouter;

Json::Array ProcessRequests(
	const Descriptions& desc,
	const TransportRouter& router,
	const TransportMap& transport_map,
	const Json::Array& requests
);
