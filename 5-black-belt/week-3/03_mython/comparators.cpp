#include "comparators.h"
#include "object.h"
#include "object_holder.h"

#include <functional>
#include <optional>
#include <sstream>
#include <cassert>

using namespace std;

namespace Runtime {

bool Equal(ObjectHolder lhs, ObjectHolder rhs) {
	Runtime::Number* lhs_num;
	Runtime::Number* rhs_num;
	if ((lhs_num = lhs.TryAs<Runtime::Number>()) && (rhs_num = rhs.TryAs<Runtime::Number>())) {
		return lhs_num->GetValue() == rhs_num->GetValue();
	}
	Runtime::String* lhs_str = lhs.TryAs<Runtime::String>();
	Runtime::String* rhs_str = rhs.TryAs<Runtime::String>();
	assert(lhs_str);
	assert(rhs_str);
	return lhs_str->GetValue() == rhs_str->GetValue();
}

bool Less(ObjectHolder lhs, ObjectHolder rhs) {
	Runtime::Number* lhs_num;
	Runtime::Number* rhs_num;
	if ((lhs_num = lhs.TryAs<Runtime::Number>()) && (rhs_num = rhs.TryAs<Runtime::Number>())) {
		return lhs_num->GetValue() < rhs_num->GetValue();
	}
	Runtime::String* lhs_str = lhs.TryAs<Runtime::String>();
	Runtime::String* rhs_str = rhs.TryAs<Runtime::String>();
	assert(lhs_str);
	assert(rhs_str);
	return lhs_str->GetValue() < rhs_str->GetValue();
}

} /* namespace Runtime */
