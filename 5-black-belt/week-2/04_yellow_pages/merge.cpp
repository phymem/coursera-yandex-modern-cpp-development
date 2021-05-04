#include "yellow_pages.h"

#include <set>

namespace YellowPages {

void MergeOptionalField(
	Company* result,
	const Signals& signals,
	const Providers& providers,
	const char* field_name
) {
	const google::protobuf::FieldDescriptor* field_desc
		= result->GetDescriptor()->FindFieldByName(field_name);
	std::string serialized_field;
	uint32_t max_priority = 0;
	for (const Signal& sig : signals) {
		if (!sig.company().GetReflection()->HasField(sig.company(), field_desc))
			continue;
		uint32_t priority = providers.at(sig.provider_id()).priority();
		if (max_priority < priority) {
			max_priority = priority;
			serialized_field.clear();
		}
		if (max_priority == priority) {
			serialized_field = sig.company().GetReflection()->GetMessage(
				sig.company(), field_desc).SerializeAsString();
		}
	}
	if (serialized_field.size()) {
		result->GetReflection()->MutableMessage(
			result, field_desc)->ParseFromString(serialized_field);
	}
}

void MergeRepeatedField(
	Company* result,
	const Signals& signals,
	const Providers& providers,
	const char* field_name
) {
	const google::protobuf::FieldDescriptor* field_desc
		= result->GetDescriptor()->FindFieldByName(field_name);
	std::set<std::string> serialized_fields;
	uint32_t max_priority = 0;
	for (const Signal& sig : signals) {
		int field_size = sig.company().GetReflection()->FieldSize(sig.company(), field_desc);
		if (!field_size)
			continue;
		uint32_t priority = providers.at(sig.provider_id()).priority();
		if (max_priority < priority) {
			max_priority = priority;
			serialized_fields.clear();
		}
		if (max_priority == priority) {
			for (int i = 0; i < field_size; ++i) {
				serialized_fields.insert(
					sig.company().GetReflection()->GetRepeatedMessage(
						sig.company(), field_desc, i).SerializeAsString());
			}
		}
	}
	for (const std::string& str : serialized_fields) {
		result->GetReflection()->AddMessage(result, field_desc)->ParseFromString(str);
	}
}

Company Merge(const Signals& signals, const Providers& providers) {
	Company result;

	MergeOptionalField(&result, signals, providers, "address");
	MergeRepeatedField(&result, signals, providers, "names");
	MergeRepeatedField(&result, signals, providers, "phones");
	MergeRepeatedField(&result, signals, providers, "urls");
	MergeOptionalField(&result, signals, providers, "working_time");

	return result;
}

}
