#include "statement.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace ast {

using runtime::Closure;
using runtime::Context;
using runtime::ObjectHolder;

namespace {
const string ADD_METHOD = "__add__"s;
const string INIT_METHOD = "__init__"s;
}  // namespace

ObjectHolder Assignment::Execute(Closure &closure, Context &context) {
	closure[var_] = move(rv_->Execute(closure, context));
	return closure[var_];
}

Assignment::Assignment(std::string var, std::unique_ptr<Statement> rv) :
		var_(var), rv_(move(rv)) {
}

VariableValue::VariableValue(const std::string &var_name) {
	dotted_ids_.push_back(var_name);
}

VariableValue::VariableValue(std::vector<std::string> dotted_ids) :
		dotted_ids_(dotted_ids) {
}

ObjectHolder VariableValue::Execute(Closure &closure, Context&) {
	Closure temp_closure = closure;
	ObjectHolder result;
	for (const auto &id : dotted_ids_) {
		if (temp_closure.count(id)) {
			result = temp_closure.at(id);
		} else {
			throw std::runtime_error("Not implemented"s);
		}
		if (result.TryAs<runtime::ClassInstance>() != nullptr) {
			temp_closure = result.TryAs<runtime::ClassInstance>()->Fields();
		}
	}
	return result;
}

unique_ptr<Print> Print::Variable(const std::string &name) {
	auto var = make_unique<VariableValue>(VariableValue(name));
	auto print = make_unique<Print>(move(var));
	return print;
}

Print::Print(unique_ptr<Statement> argument) {
	args_.push_back(move(argument));
}

Print::Print(vector<unique_ptr<Statement>> args) :
		args_(move(args)) {
}
void Print::ExecutePrint(size_t pos, runtime::Closure &closure,
		runtime::Context &context) {
	auto executed_arg = args_[pos]->Execute(closure, context);
	if (executed_arg) {
		executed_arg.Get()->Print(context.GetOutputStream(), context);
	} else {
		context.GetOutputStream() << "None"s;
	}
}
ObjectHolder Print::Execute(Closure &closure, Context &context) {
	if (!args_.empty()) {
		size_t i = 0;
		for (; i < args_.size() - 1; ++i) {
			ExecutePrint(i, closure, context);
			context.GetOutputStream() << " "s;
		}
		ExecutePrint(args_.size() - 1, closure, context);
	}
	context.GetOutputStream() << endl;
	return ObjectHolder();
}

MethodCall::MethodCall(std::unique_ptr<Statement> object, std::string method,
		std::vector<std::unique_ptr<Statement>> args) :
		object_(move(object)), method_(method), args_(move(args)) {
}

ObjectHolder MethodCall::Execute(Closure &closure, Context &context) {
	std::vector<ObjectHolder> args_converted;
	for (size_t i = 0; i < args_.size(); ++i) {
		args_converted.push_back(
				move(args_[i].get()->Execute(closure, context)));
	}
	return object_.get()->Execute(closure, context).TryAs<runtime::ClassInstance>()->Call(
			method_, args_converted, context);
}

ObjectHolder Stringify::Execute(Closure &closure, Context &context) {
	ostringstream string_stream;
	if (argument_.get()->Execute(closure, context)) {
		argument_.get()->Execute(closure, context)->Print(string_stream,
				context);
	} else {
		string_stream << "None"s;
	}
	string str = string_stream.str();
	return ObjectHolder::Own(runtime::String(str));
}

ObjectHolder Add::Execute(Closure &closure, Context &context) {
	const auto &lhs = lhs_.get()->Execute(closure, context);
	const auto &rhs = rhs_.get()->Execute(closure, context);

	if (lhs.TryAs<runtime::String>() != nullptr
			&& rhs.TryAs<runtime::String>() != nullptr) {
		const auto &lhs_val = lhs.TryAs<runtime::String>()->GetValue();
		const auto &rhs_val = rhs.TryAs<runtime::String>()->GetValue();
		return ObjectHolder::Own(runtime::String(lhs_val + rhs_val));
	}
	if (lhs.TryAs<runtime::Number>() != nullptr
			&& rhs.TryAs<runtime::Number>() != nullptr) {
		const auto &lhs_val = lhs.TryAs<runtime::Number>()->GetValue();
		const auto &rhs_val = rhs.TryAs<runtime::Number>()->GetValue();
		return ObjectHolder::Own(runtime::Number(lhs_val + rhs_val));
	}
	if (lhs.TryAs<runtime::ClassInstance>() != nullptr) {
		const auto &lhs_cl = lhs.TryAs<runtime::ClassInstance>();
		if (lhs_cl->HasMethod("__add__"s, 1)) {
			Closure args_table_;
			string argument_name = "rhs"s;
			args_table_[argument_name] = ObjectHolder::Share(*rhs);
			return lhs_cl->Call("__add__"s, { rhs }, context);
		}
	}
	throw std::runtime_error("Cannot compare objects for equality"s);
}

ObjectHolder Sub::Execute(Closure &closure, Context &context) {
	const auto &lhs = lhs_.get()->Execute(closure, context);
	const auto &rhs = rhs_.get()->Execute(closure, context);
	if (lhs.TryAs<runtime::Number>() != nullptr
			&& rhs.TryAs<runtime::Number>() != nullptr) {
		const auto &lhs_val = lhs.TryAs<runtime::Number>()->GetValue();
		const auto &rhs_val = rhs.TryAs<runtime::Number>()->GetValue();
		return ObjectHolder::Own(runtime::Number(lhs_val - rhs_val));
	}
	throw std::runtime_error("Cannot compare objects for equality"s);
}

ObjectHolder Mult::Execute(Closure &closure, Context &context) {
	const auto &lhs = lhs_.get()->Execute(closure, context);
	const auto &rhs = rhs_.get()->Execute(closure, context);
	if (lhs.TryAs<runtime::Number>() != nullptr
			&& rhs.TryAs<runtime::Number>() != nullptr) {
		const auto &lhs_val = lhs.TryAs<runtime::Number>()->GetValue();
		const auto &rhs_val = rhs.TryAs<runtime::Number>()->GetValue();
		return ObjectHolder::Own(runtime::Number(lhs_val * rhs_val));
	}
	throw std::runtime_error("Cannot compare objects for equality"s);
}

ObjectHolder Div::Execute(Closure &closure, Context &context) {
	const auto &lhs = lhs_.get()->Execute(closure, context);
	const auto &rhs = rhs_.get()->Execute(closure, context);
	if (lhs.TryAs<runtime::Number>() != nullptr
			&& rhs.TryAs<runtime::Number>() != nullptr) {
		const auto &lhs_val = lhs.TryAs<runtime::Number>()->GetValue();
		const auto &rhs_val = rhs.TryAs<runtime::Number>()->GetValue();
		if (rhs_val != 0) {
			return ObjectHolder::Own(runtime::Number(lhs_val / rhs_val));
		}
	}
	throw std::runtime_error("Cannot compare objects for equality"s);
}

ObjectHolder Compound::Execute(Closure &closure, Context &context) {
	for (const auto &statement : compound_) {
		statement.get()->Execute(closure, context);
	}
	return ObjectHolder();
}

ObjectHolder Return::Execute(Closure& /*closure*/, Context& /*context*/) {
	throw ReturnException(statement_);
	return ObjectHolder();
}

ClassDefinition::ClassDefinition(ObjectHolder cls) :
		cls_(cls) {
}

ObjectHolder ClassDefinition::Execute(Closure &closure, Context&) {
	closure[cls_.TryAs<runtime::Class>()->GetName()] = ObjectHolder::Own(
			runtime::ClassInstance(*cls_.TryAs<runtime::Class>()));
	return ObjectHolder();
}

FieldAssignment::FieldAssignment(VariableValue object, std::string field_name,
		std::unique_ptr<Statement> rv) :
		object_(object), field_name_(field_name), rv_(move(rv)) {
}

ObjectHolder FieldAssignment::Execute(Closure &closure, Context &context) {
	ObjectHolder result;
	result = object_.Execute(closure, context);
	result = result.TryAs<runtime::ClassInstance>()->Fields()[field_name_] =
			rv_->Execute(closure, context);
	return result;
}

IfElse::IfElse(std::unique_ptr<Statement> condition,
		std::unique_ptr<Statement> if_body,
		std::unique_ptr<Statement> else_body) :
		condition_(move(condition)), if_body_(move(if_body)), else_body_(
				move(else_body)) {
}

ObjectHolder IfElse::Execute(Closure &closure, Context &context) {
	if (condition_.get()->Execute(closure, context).TryAs<runtime::Bool>()->GetValue()) {
		if (if_body_.get() != nullptr) {
			return if_body_->Execute(closure, context);
		}
	} else {
		if (else_body_.get() != nullptr) {
			return else_body_->Execute(closure, context);
		}
	}
	return {};
}

ObjectHolder Or::Execute(Closure &closure, Context &context) {
	const auto &lhs = lhs_.get()->Execute(closure, context);
	runtime::Bool true_obj(true);
	if (runtime::Equal(lhs, ObjectHolder::Share(true_obj), context)) {
		return ObjectHolder::Own(runtime::Bool(true));
	}
	const auto &rhs = rhs_.get()->Execute(closure, context);
	if (runtime::Equal(rhs, ObjectHolder::Share(true_obj), context)) {
		return ObjectHolder::Own(runtime::Bool(true));
	}
	return ObjectHolder::Own(runtime::Bool(false));
}

ObjectHolder And::Execute(Closure &closure, Context &context) {
	const auto &lhs = lhs_.get()->Execute(closure, context);
	runtime::Bool true_obj(true);
	if (!runtime::Equal(lhs, ObjectHolder::Share(true_obj), context)) {
		return ObjectHolder::Own(runtime::Bool(false));
	}
	const auto &rhs = rhs_.get()->Execute(closure, context);
	if (!runtime::Equal(rhs, ObjectHolder::Share(true_obj), context)) {
		return ObjectHolder::Own(runtime::Bool(false));
	}
	return ObjectHolder::Own(runtime::Bool(true));
}

ObjectHolder Not::Execute(Closure &closure, Context &context) {
	const auto &argument = argument_.get()->Execute(closure, context);
	runtime::Bool true_obj(true);
	if (runtime::Equal(argument, ObjectHolder::Share(true_obj), context)) {
		return ObjectHolder::Own(runtime::Bool(false));
	}
	return ObjectHolder::Own(runtime::Bool(true));
}

Comparison::Comparison(Comparator cmp, unique_ptr<Statement> lhs,
		unique_ptr<Statement> rhs) :
		BinaryOperation(std::move(lhs), std::move(rhs)), comparator_(move(cmp)) {
}

ObjectHolder Comparison::Execute(Closure &closure, Context &context) {
	if (comparator_(lhs_.get()->Execute(closure, context),
			rhs_.get()->Execute(closure, context), context)) {
		return ObjectHolder::Own(runtime::Bool(true));
	} else {
		return ObjectHolder::Own(runtime::Bool(false));
	}
	return {};
}

NewInstance::NewInstance(const runtime::Class &class_,
		std::vector<std::unique_ptr<Statement>> args) :
		new_class_(class_), args_(move(args)) {
}

NewInstance::NewInstance(const runtime::Class &class_) :/*clas_ptr(std::make_shared<runtime::Class>(class_)),*/
		new_class_(class_) {
}

ObjectHolder NewInstance::Execute(Closure &closure, Context &context) {
	if (new_class_.HasMethod("__init__"s, args_.size())) {
		vector<ObjectHolder> param_vect;
		for (const auto &param : args_) {
			param_vect.push_back(param.get()->Execute(closure, context));
		}
		new_class_.Call("__init__"s, param_vect, context);
		return ObjectHolder::Share(new_class_);
	}
	return ObjectHolder::Share(new_class_);
}

MethodBody::MethodBody(std::unique_ptr<Statement> &&body) :
		body_(move(body)) {
}

ObjectHolder MethodBody::Execute(Closure &closure, Context &context) {
	try {
		return body_->Execute(closure, context);
	} catch (ReturnException &e) {
		return e.GetReturn()->Execute(closure, context);
	}
	return ObjectHolder();
}

}  // namespace ast
