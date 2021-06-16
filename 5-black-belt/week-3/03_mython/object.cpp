#include "object.h"
#include "statement.h"

#include <sstream>
#include <string_view>
#include <cassert>

using namespace std;

namespace Runtime {

void ClassInstance::Print(std::ostream& os) {
	if (HasMethod("__str__"s, 0)) {
		Call("__str__", {})->Print(os);
	}
	else {
		os << this;
	}
}

bool ClassInstance::HasMethod(const std::string& method_name, size_t argument_count) const {
	const Method* method = class_.GetMethod(method_name);
	return method && method->formal_params.size() == argument_count;
}

const Closure& ClassInstance::Fields() const {
	return fields;
}

Closure& ClassInstance::Fields() {
	return fields;
}

ClassInstance::ClassInstance(const Class& class__)
	: class_(class__)
{
}

ObjectHolder ClassInstance::Call(
	const std::string& method_name
	, const std::vector<ObjectHolder>& actual_args
) {
	const Method* method = class_.GetMethod(method_name);
	assert(method);

	Closure closure = {{"self", ObjectHolder::Share(*this)}};

	auto p_it = method->formal_params.begin();
	auto p_end = method->formal_params.end();
	auto a_it = actual_args.begin();
	auto a_end = actual_args.end();
	assert(method->formal_params.size() == actual_args.size());
	for (; p_it != p_end && a_it != a_end; ++p_it, ++a_it) {
		assert(!closure.count(*p_it) && "*p_it is already in the closure");
		closure[*p_it] = *a_it;
	}

	ObjectHolder retval;
	try {
		retval = method->body->Execute(closure);
	}
	catch (ObjectHolder& return_result) { // XXX thrown in Ast::Return::Execute()
		retval = return_result;
	}

	return retval;
}

Class::Class(
	std::string name_
	, std::vector<Method> methods_
	, const Class* parent_
)
	: name(std::move(name_))
	, methods(std::move(methods_))
	, parent(parent_)
{
}

const Method* Class::GetMethod(const std::string& name_) const {
	for (const Method& m : methods) {
		if (m.name == name_)
			return &m;
	}
	return parent ? parent->GetMethod(name_) : nullptr;
}

void Class::Print(ostream& os) {
	os << "Class " << name;
}

const std::string& Class::GetName() const {
	return name;
}

void Bool::Print(std::ostream& os) {
	os << (GetValue() ? "True" : "False");
}

} /* namespace Runtime */
