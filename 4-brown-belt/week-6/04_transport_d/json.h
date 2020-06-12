#pragma once

#include <istream>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include <cstdlib>

namespace Json {

	class Node : std::variant<
					std::vector<Node>,
					std::map<std::string, Node>,
					std::string> {
	public:
		using variant::variant;

		const auto& AsArray() const {
			return std::get<std::vector<Node>>(*this);
		}
		const auto& AsMap() const {
			return std::get<std::map<std::string, Node>>(*this);
		}
		const auto& AsString() const {
			return std::get<std::string>(*this);
		}
		int AsInt() const {
			return std::stoi(std::get<std::string>(*this));
		}
		double AsDouble() const {
			return std::strtod(std::get<std::string>(*this).c_str(), nullptr);
		}
	};

	class Document {
	public:
		explicit Document(Node root);

		const Node& GetRoot() const;

	private:
		Node root;
	};

	Document Load(std::istream& input);
}
