#include "statement.h"
#include "object.h"

#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;

namespace Ast {

using Runtime::Closure;

vector<ObjectHolder> ExecuteArgs(const vector<unique_ptr<Statement>>& args, Closure& closure) {
	vector<ObjectHolder> ret;
	ret.reserve(args.size());
	for (auto& st : args) {
		ret.push_back(st->Execute(closure));
	}
	return ret;
}

Assignment::Assignment(
	std::string var_name_
	, std::unique_ptr<Statement> rv
)
	: var_name(std::move(var_name_))
	, right_value(std::move(rv))
{
}

ObjectHolder Assignment::Execute(Closure& closure) {
	return closure[var_name] = right_value->Execute(closure);
}

VariableValue::VariableValue(std::string var_name) {
	dotted_ids.push_back(var_name);
}

VariableValue::VariableValue(std::vector<std::string> dotted_ids_)
	: dotted_ids(std::move(dotted_ids_))
{
}

ObjectHolder VariableValue::Execute(Closure& closure) {
	Closure* current = &closure;
	for (size_t i = 0; i < (dotted_ids.size() - 1); ++i) {
		Closure::iterator it = current->find(dotted_ids[i]);
		if (it == closure.end()) {
			throw std::runtime_error{"VariableValue::Execute - not found (dotted_ids)"};
		}
		Runtime::ClassInstance* inst = it->second.TryAs<Runtime::ClassInstance>();
		assert(inst && "VariableValue::Execute");
		current = &inst->Fields();
	}
	if (Closure::iterator it = current->find(dotted_ids.back()); it != current->end())
		return it->second;
	throw std::runtime_error{"VariableValue::Execute - obj not found"};
}

unique_ptr<Print> Print::Variable(std::string var) {
	return make_unique<Print>(make_unique<VariableValue>(var));
}

Print::Print(unique_ptr<Statement> arg) {
	args.push_back(std::move(arg));
}

Print::Print(vector<unique_ptr<Statement>> args_)
	: args(std::move(args_))
{
}

ObjectHolder Print::Execute(Closure& closure) {
	const char* delim = "";
	for (auto& st : args) {
		*output << delim;
		delim = " ";
		ObjectHolder holder = st->Execute(closure);
		if (Runtime::Object* obj = holder.Get()) {
			obj->Print(*output);
		}
		else {
			*output << "None";
		}
	}
	*output << '\n';
	return ObjectHolder::None();
}

ostream* Print::output = &cout;

void Print::SetOutputStream(ostream& output_stream) {
	 output = &output_stream;
}

MethodCall::MethodCall(
	std::unique_ptr<Statement> object_
	, std::string method_
	, std::vector<std::unique_ptr<Statement>> args_
)
	: object(std::move(object_))
	, method(std::move(method_))
	, args(std::move(args_))
{
}

ObjectHolder MethodCall::Execute(Closure& closure) {
	ObjectHolder inst_obj = object->Execute(closure);
	Runtime::ClassInstance* inst = inst_obj.TryAs<Runtime::ClassInstance>();
	assert(inst);
	return inst->Call(method, ExecuteArgs(args, closure));
}

ObjectHolder Stringify::Execute(Closure& closure) {
	ObjectHolder holder = argument->Execute(closure);
	assert(holder.Get());
	std::ostringstream output;
	holder->Print(output);
	return ObjectHolder::Own(Runtime::String(std::move(output.str())));
}

ObjectHolder Add::Execute(Closure& closure) {
	ObjectHolder lhs_ = lhs->Execute(closure);
	ObjectHolder rhs_ = rhs->Execute(closure);

	Runtime::Number* l_num;
	Runtime::Number* r_num;
	if ((l_num = lhs_.TryAs<Runtime::Number>()) && (r_num = rhs_.TryAs<Runtime::Number>())) {
		return ObjectHolder::Own(Runtime::Number(l_num->GetValue() + r_num->GetValue()));
	}

	Runtime::String* l_str;
	Runtime::String* r_str;
	if ((l_str = lhs_.TryAs<Runtime::String>()) && (r_str = rhs_.TryAs<Runtime::String>())) {
		return ObjectHolder::Own(Runtime::String(l_str->GetValue() + r_str->GetValue()));
	}

	Runtime::ClassInstance* l_inst = lhs_.TryAs<Runtime::ClassInstance>();
	if (l_inst && l_inst->HasMethod("__add__", 1)) {
		return l_inst->Call("__add__", { rhs_ });
	}

	throw std::runtime_error{"Add::Execute - lhs/rhs type mismatch"};
}

ObjectHolder Sub::Execute(Closure& closure) {
	ObjectHolder lhs_ = lhs->Execute(closure);
	Runtime::Number* l_num = lhs_.TryAs<Runtime::Number>();
	assert(l_num);
	ObjectHolder rhs_ = rhs->Execute(closure);
	Runtime::Number* r_num = rhs_.TryAs<Runtime::Number>();
	assert(r_num);
	return ObjectHolder::Own(Runtime::Number(l_num->GetValue() - r_num->GetValue()));
}

ObjectHolder Mult::Execute(Runtime::Closure& closure) {
	ObjectHolder lhs_ = lhs->Execute(closure);
	Runtime::Number* l_num = lhs_.TryAs<Runtime::Number>();
	assert(l_num);
	ObjectHolder rhs_ = rhs->Execute(closure);
	Runtime::Number* r_num = rhs_.TryAs<Runtime::Number>();
	assert(r_num);
	return ObjectHolder::Own(Runtime::Number(l_num->GetValue() * r_num->GetValue()));
}

ObjectHolder Div::Execute(Runtime::Closure& closure) {
	ObjectHolder lhs_ = lhs->Execute(closure);
	Runtime::Number* l_num = lhs_.TryAs<Runtime::Number>();
	assert(l_num);
	ObjectHolder rhs_ = rhs->Execute(closure);
	Runtime::Number* r_num = rhs_.TryAs<Runtime::Number>();
	assert(r_num);
	return ObjectHolder::Own(Runtime::Number(l_num->GetValue() / r_num->GetValue()));
}

ObjectHolder Compound::Execute(Closure& closure) {
	for (auto& st : statements) {
		st->Execute(closure);
	}
	return ObjectHolder::None();
}

ObjectHolder Return::Execute(Closure& closure) {
	throw statement->Execute(closure); // XXX caught in Runtime::ClassInstance::Call()
}

ClassDefinition::ClassDefinition(ObjectHolder class__)
	: class_(std::move(class__))
{
	assert(class_.TryAs<Runtime::Class>());
}

ObjectHolder ClassDefinition::Execute(Runtime::Closure& closure) {
	return ObjectHolder::None(); // ???
}

FieldAssignment::FieldAssignment(
	VariableValue object_
	, std::string field_name_
	, std::unique_ptr<Statement> rv
)
	: object(std::move(object_))
	, field_name(std::move(field_name_))
	, right_value(std::move(rv))
{
}

ObjectHolder FieldAssignment::Execute(Runtime::Closure& closure) {
	ObjectHolder obj = object.Execute(closure);
	Runtime::ClassInstance* inst = obj.TryAs<Runtime::ClassInstance>();
	assert(inst);
	return inst->Fields()[field_name] = right_value->Execute(closure);
}

IfElse::IfElse(
	std::unique_ptr<Statement> condition_
	, std::unique_ptr<Statement> if_body_
	, std::unique_ptr<Statement> else_body_
)
	: condition(std::move(condition_))
	, if_body(std::move(if_body_))
	, else_body(std::move(else_body_))
{
}

ObjectHolder IfElse::Execute(Runtime::Closure& closure) {
	if (IsTrue(condition->Execute(closure))) {
		return if_body->Execute(closure);
	}
	ObjectHolder retval = ObjectHolder::None();
	if (else_body.get()) {
		retval = else_body->Execute(closure);
	}
	return retval;
}

ObjectHolder Or::Execute(Runtime::Closure& closure) {
	ObjectHolder lhs_ = lhs->Execute(closure);
	ObjectHolder rhs_ = rhs->Execute(closure);
	return ObjectHolder::Own(Runtime::Bool(IsTrue(lhs_) || IsTrue(rhs_)));
}

ObjectHolder And::Execute(Runtime::Closure& closure) {
	ObjectHolder lhs_ = lhs->Execute(closure);
	ObjectHolder rhs_ = rhs->Execute(closure);
	return ObjectHolder::Own(Runtime::Bool(IsTrue(lhs_) && IsTrue(rhs_)));
}

ObjectHolder Not::Execute(Runtime::Closure& closure) {
	return ObjectHolder::Own(Runtime::Bool(!IsTrue(argument->Execute(closure))));
}

Comparison::Comparison(
	Comparator cmp
	, unique_ptr<Statement> lhs
	, unique_ptr<Statement> rhs
)
	: comparator(std::move(cmp))
	, left(std::move(lhs))
	, right(std::move(rhs))
{
}

ObjectHolder Comparison::Execute(Runtime::Closure& closure) {
	ObjectHolder lhs = left->Execute(closure);
	ObjectHolder rhs = right->Execute(closure);
	return ObjectHolder::Own(Runtime::Bool(comparator(lhs, rhs)));
}

NewInstance::NewInstance(
	const Runtime::Class& class__
	, std::vector<std::unique_ptr<Statement>> args_
)
	: class_(class__)
	, args(std::move(args_))
{
}

NewInstance::NewInstance(const Runtime::Class& class__)
	: class_(class__)
{
}

ObjectHolder NewInstance::Execute(Runtime::Closure& closure) {
	ObjectHolder ret = ObjectHolder::Own(Runtime::ClassInstance(class_));
	Runtime::ClassInstance* inst = ret.TryAs<Runtime::ClassInstance>();
	assert(inst);
	if (args.empty()) {
		if (inst->HasMethod("__init__", 0)) {
			inst->Call("__init__", {});
		}
	}
	else {
		assert(inst->HasMethod("__init__", args.size()));
		inst->Call("__init__", ExecuteArgs(args, closure));
	}
	return ret;
}

} /* namespace Ast */
