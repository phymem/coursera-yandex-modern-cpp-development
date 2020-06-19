#pragma once

#include <cstring> // strcmp

#include "readbuf.h"

class JsonParser {
public:

	const size_t MAX_BUS_STOPS = 250;

	struct BusStopData {
		char m_buf[256];
		int m_distance;
		std::string_view m_name;
	};
	typedef std::vector<BusStopData> bus_stop_vec_t;

	class Callbacks {
	public:
		virtual void setRoutingSettings(int f_bus_wait_time, double f_bus_velocity) = 0;

		virtual void addBusRoute(std::string_view f_name,
			bool f_roundtrip, const bus_stop_vec_t& f_stops) = 0;
		virtual void addBusStop(std::string_view f_name,
			double f_longitude, double f_latitude, const bus_stop_vec_t& f_distances) = 0;

		virtual void reportBus(std::string_view f_name) = 0;
		virtual void reportStop(std::string_view f_name) = 0;
		virtual void reportRoute(std::string_view f_from, std::string_view f_to) = 0;
	};

	JsonParser(Callbacks* f_callbacks) :
		m_callbacks(f_callbacks) { m_stops.reserve(MAX_BUS_STOPS); }

	void parse() {
		if (skip_space() != '{')
			throw std::invalid_argument("JsonParser: '{'");

		while (true) {
			char b[64];
			ReadBuf buf(b, sizeof(b));
			buf.readString();

			if (skip_space() != ':')
				throw std::invalid_argument("JsonParser: ':'");

			if (!strcmp(buf.m_buf, "routing_settings")) {
				if (skip_space() != '{')
					throw std::invalid_argument("JsonParser: routing_settings '{'");

				parseRoutingSettings();

				if (skip_space() != '}')
					throw std::invalid_argument("JsonParser: routing_settings '}'");
			}
			else if (!strcmp(buf.m_buf, "base_requests")) {
				if (skip_space() != '[')
					throw std::invalid_argument("JsonParser: '['");

				while (true) {
					if (skip_space() != '{')
						throw std::invalid_argument("JsonParser: '{'");

					parseBaseRequest();

					if (skip_space() != '}')
						throw std::invalid_argument("JsonParser: '}'");

					char ch = skip_space();
					if (ch == ',')
						continue;
					if (ch == ']')
						break;

					throw std::invalid_argument("JsonParser: invalid delim");
				}
			}
			else if (!strcmp(buf.m_buf, "stat_requests")) {
				if (skip_space() != '[')
					throw std::invalid_argument("JsonParser: '['");

				fprintf(stdout, "[\n");

				while (true) {
					if (skip_space() != '{')
						throw std::invalid_argument("JsonParser: '{'");

					fprintf(stdout, "  {\n");

					parseStatRequest();

					if (skip_space() != '}')
						throw std::invalid_argument("JsonParser: '}'");

					char ch = skip_space();
					if (ch == ',') {
						fprintf(stdout, "  },\n");
						continue;
					}
					if (ch == ']') {
						fprintf(stdout, "  }\n]\n");
						break;
					}

					throw std::invalid_argument("JsonParser: invalid delim");
				}
			}
			else {
				throw std::invalid_argument("JsonParser: invalid request");
			}

			char ch = skip_space();
			if (ch == ',')
				continue;
			if (ch == '}')
				break;

			throw std::invalid_argument("JsonParser: invalid delim");
		}
	}

private:

	void parseRoutingSettings() {
		int wait_time = 0;
		double velocity = 0;

		while (true) {
			char b[128];
			ReadBuf buf(b, sizeof(b));
			buf.readString();

			if (skip_space() != ':')
				throw std::invalid_argument("JsonParser: ':'");

			if (!strcmp(buf.m_buf, "bus_wait_time"))
				wait_time = buf.readNumber().to_int();
			else if (!strcmp(buf.m_buf, "bus_velocity"))
				velocity = buf.readNumber().to_double();
			else
				throw std::invalid_argument("parseRoutingSettings: invalid key");

			char ch = skip_space();
			if (ch == ',') {
				continue;
			}
			if (ch != EOF) {
				ungetc(ch, stdin);
			}
			break;
		}

		m_callbacks->setRoutingSettings(wait_time, velocity);
	}

	void parseBaseRequest() {
		enum ReqType {
			RT_NONE,
			RT_BUS,
			RT_STOP
		};
		ReqType req_type = RT_NONE;

		double longitude = 0;
		double latitude = 0;

		m_stops.clear();
		char bn[256];
		ReadBuf name(bn, sizeof(bn));

		bool roundtrip = false;
		char ch;

		while (true) {
			char b[256];
			ReadBuf buf(b, sizeof(b));
			buf.readString();

			if (skip_space() != ':')
				throw std::invalid_argument("parseBaseRequest: ':'");

			if (!strcmp(buf.m_buf, "name")) {
				name.readString();
			}
			else if (!strcmp(buf.m_buf, "longitude")) {
				longitude = buf.readNumber().to_double();
			}
			else if (!strcmp(buf.m_buf, "latitude")) {
				latitude = buf.readNumber().to_double();
			}
			else if (!strcmp(buf.m_buf, "is_roundtrip")) {
				const char* p_true = "true";
				const char* p_false = "false";
				const char* p = nullptr;
				switch (skip_space()) {
				case 't':
					p = p_true;
					roundtrip = true;
					break;
				case 'f':
					p = p_false;
					roundtrip = false;
					break;
				default:
					throw std::invalid_argument("parseBaseRequest: is_roundtrip");
				}

				for (++p; *p && *p == getchar(); ++p);

				if (*p)
					throw std::invalid_argument("parseBaseRequest: is_roundtrip");
			}
			else if (!strcmp(buf.m_buf, "type")) {
				buf.readString();
				if (!strcmp(buf.m_buf, "Bus"))
					req_type = RT_BUS;
				else if (!strcmp(buf.m_buf, "Stop"))
					req_type = RT_STOP;
				else
					throw std::invalid_argument("parseBaseRequest: type");
			}
			else if (!strcmp(buf.m_buf, "stops")) {
				if (skip_space() != '[')
					throw std::invalid_argument("parseBaseRequest: stops '['");

				while (true) {
					m_stops.push_back(BusStopData());
					m_stops.back().m_name = ReadBuf(
						m_stops.back().m_buf, sizeof(m_stops.back().m_buf)).readString().to_string_view();

					ch = skip_space();
					if (ch == ',')
						continue;
					if (ch == ']')
						break;

					throw std::invalid_argument("parseBaseRequest: stops invalid delim");
				}
			}
			else if (!strcmp(buf.m_buf, "road_distances")) {
				if (skip_space() != '{')
					throw std::invalid_argument("parseBaseRequest: distances '{'");

				ch = skip_space();
				if (ch != '}') {
					ungetc(ch, stdin);
					while (true) {
						m_stops.push_back(BusStopData());
						m_stops.back().m_name = ReadBuf(
							m_stops.back().m_buf, sizeof(m_stops.back().m_buf)).readString().to_string_view();

						if (skip_space() != ':')
							throw std::invalid_argument("parseBaseRequest: distances ':'");

						m_stops.back().m_distance = buf.readNumber().to_int();

						ch = skip_space();
						if (ch == ',')
							continue;
						if (ch == '}')
							break;

						throw std::invalid_argument("parseBaseRequest: distances invalid delim");
					}
				}
			}
			else {
				throw std::invalid_argument("parseBaseRequest: invalid key");
			}

			ch = skip_space();
			if (ch == ',') {
				continue;
			}
			if (ch != EOF) {
				ungetc(ch, stdin);
			}
			break;
		}

		switch (req_type) {
		case RT_STOP:
			m_callbacks->addBusStop(name.to_string_view(), longitude, latitude, m_stops);
			break;
		case RT_BUS:
			m_callbacks->addBusRoute(name.to_string_view(), roundtrip, m_stops);
			break;
		default:
			throw std::invalid_argument("parseBaseRequest: invalid type");
		}
	}

	void parseStatRequest() {
		char b1[256];
		ReadBuf name(b1, sizeof(b1));

		char b2[256];
		ReadBuf stop_to(b2, sizeof(b2));

		char b3[256];
		ReadBuf stop_from(b3, sizeof(b3));

		char b4[256];
		ReadBuf id(b4, sizeof(b4));

		enum ReqType {
			RT_NONE,
			RT_BUS,
			RT_STOP,
			RT_ROUTE
		};
		ReqType req_type = RT_NONE;

		while (true) {
			char b[256];
			ReadBuf buf(b, sizeof(b));
			buf.readString();

			if (skip_space() != ':')
				throw std::invalid_argument("parseStatRequest: ':'");

			if (!strcmp(buf.m_buf, "name")) {
				name.readString();
			}
			else if (!strcmp(buf.m_buf, "to")) {
				stop_to.readString();
			}
			else if (!strcmp(buf.m_buf, "from")) {
				stop_from.readString();
			}
			else if (!strcmp(buf.m_buf, "id")) {
				id.read([](char f_ch) { return isdigit(f_ch); });
			}
			else if (!strcmp(buf.m_buf, "type")) {
				buf.readString();
				if (!strcmp(buf.m_buf, "Bus"))
					req_type = RT_BUS;
				else if (!strcmp(buf.m_buf, "Stop"))
					req_type = RT_STOP;
				else if (!strcmp(buf.m_buf, "Route"))
					req_type = RT_ROUTE;
				else
					throw std::invalid_argument("parseStatRequest: type");
			}
			else {
				throw std::invalid_argument("parseStatRequest: invalid key");
			}

			char ch = skip_space();
			if (ch == ',') {
				continue;
			}
			if (ch != EOF) {
				ungetc(ch, stdin);
			}
			break;
		}

		fprintf(stdout, "    \"request_id\": %s,\n", id.m_buf);

		switch (req_type) {
		case RT_BUS:
			m_callbacks->reportBus(name.to_string_view());
			break;
		case RT_STOP:
			m_callbacks->reportStop(name.to_string_view());
			break;
		case RT_ROUTE:
			m_callbacks->reportRoute(
				stop_from.to_string_view(),
				stop_to.to_string_view());
			break;
		default:
			throw std::invalid_argument("parseStatRequest: type");
		}
	}

private:

	Callbacks* m_callbacks;
	bus_stop_vec_t m_stops;
};

