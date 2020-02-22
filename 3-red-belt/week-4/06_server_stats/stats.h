#pragma once

#include "http_request.h"

#include <string_view>
#include <map>
using namespace std;

class Stats {
public:

	void AddMethod(string_view method) {
		auto it = m_methods.find(method);
		if (it == end(m_methods))
			m_methods["UNKNOWN"]++;
		else
			it->second++;
	}

	void AddUri(string_view uri) {
		auto it = m_URIs.find(uri);
		if (it == end(m_URIs))
			m_URIs["unknown"]++;
		else
			it->second++;
	}

	const map<string_view, int>& GetMethodStats() const { return m_methods; }
	const map<string_view, int>& GetUriStats() const { return m_URIs; }

private:

	map<string_view, int> m_methods {
		{ "GET", 0 },
		{ "POST", 0 },
		{ "PUT", 0 },
		{ "DELETE", 0 },
		{ "UNKNOWN", 0 },
	};

	map<string_view, int> m_URIs {
		{ "/", 0 },
		{ "/order", 0 },
		{ "/product", 0 },
		{ "/basket", 0 },
		{ "/help", 0 },
		{ "unknown", 0 },
	};
};

HttpRequest ParseRequest(string_view line);
