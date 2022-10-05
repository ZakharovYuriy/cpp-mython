#include "runtime.h"

#include <algorithm>
#include <cassert>
#include <optional>
#include <sstream>

using namespace std;

namespace runtime {

ObjectHolder::ObjectHolder(std::shared_ptr<Object> data)
    : data_(std::move(data)) {
}

void ObjectHolder::AssertIsValid() const {
    assert(data_ != nullptr);
}

ObjectHolder ObjectHolder::Share(Object& object) {
    // Возвращаем невладеющий shared_ptr (его deleter ничего не делает)
    return ObjectHolder(std::shared_ptr<Object>(&object, [](auto* /*p*/) { /* do nothing */ }));
}

ObjectHolder ObjectHolder::None() {
    return ObjectHolder();
}

Object& ObjectHolder::operator*() const {
    AssertIsValid();
    return *Get();
}

Object* ObjectHolder::operator->() const {
    AssertIsValid();
    return Get();
}

Object* ObjectHolder::Get() const {
    return data_.get();
}

ObjectHolder::operator bool() const {
    return Get() != nullptr;
}

bool IsTrue(const ObjectHolder& object) {
    if (object.TryAs<String>() != nullptr) {
        return !object.TryAs<String>()->GetValue().empty();
    }
    if (object.TryAs<Number>() != nullptr) {
        return object.TryAs<Number>()->GetValue() !=0? true:false;
    }
    if (object.TryAs<Bool>() != nullptr) {
        return object.TryAs<Bool>()->GetValue();
    }
    return false;
}

void ClassInstance::Print(std::ostream& os, Context& context) {
    if (cls_.GetMethod("__str__"s) != nullptr) {
        Call("__str__"s, {}, context).Get()->Print(os, context);
    }
    else {
        os << this;
    }
}

bool ClassInstance::HasMethod(const std::string& method, size_t argument_count) const {
    auto getm_metod = cls_.GetMethod(method);
    if (getm_metod != nullptr) {
        if (getm_metod->formal_params.size() == argument_count) {
            return true;
        }
    }
    return false;
}

Closure& ClassInstance::Fields() {
    return fields_table_;
}

const Closure& ClassInstance::Fields() const {
    return fields_table_;
}

ClassInstance::ClassInstance(const Class& cls):cls_(cls) {
}

ObjectHolder ClassInstance::Call(const std::string& method,
                                 const std::vector<ObjectHolder>& actual_args,
                                 Context& context) {
    if (HasMethod(method, actual_args.size())) {
        Closure args_table_;
        size_t i = 0;
        for (const auto& arg_name : cls_.GetMethod(method)->formal_params) {
            args_table_[arg_name] = actual_args[i++];
        }       
        args_table_["self"s] = ObjectHolder::Share(*this);
        return cls_.GetMethod(method)->body.get()->Execute(args_table_, context);
    }
    else {
        throw std::runtime_error("Not implemented"s);
    }
}

Class::Class(std::string name, std::vector<Method> methods, const Class* parent):
    name_(name), methods_(move(methods)), parent_(parent)
{
}

const Method* Class::GetMethod(const std::string& name) const {
    auto name_check = [name](const Method& method) { return method.name == name; };
    auto result = find_if(methods_.begin(), methods_.end(), name_check);
    if (result != methods_.end()) {

        return &(*result);
    }
    else {
        if (parent_ != nullptr) {
            return parent_->GetMethod(name);
        }
    }
    return nullptr;
}

[[nodiscard]] const std::string& Class::GetName() const {
    return name_;
}

void Class::Print(ostream& os, [[maybe_unused]] Context& context) {
    os << "Class "s << name_;
}

void Bool::Print(std::ostream& os, [[maybe_unused]] Context& context) {
    os << (GetValue() ? "True"sv : "False"sv);
}

bool Equal(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context) {
    if (lhs.TryAs<String>() != nullptr && rhs.TryAs<String>() != nullptr) {
        return lhs.TryAs<String>()->GetValue() == rhs.TryAs<String>()->GetValue();
    }
    if (lhs.TryAs<Number>() != nullptr && rhs.TryAs<Number>() != nullptr) {
        return lhs.TryAs<Number>()->GetValue() == rhs.TryAs<Number>()->GetValue();
    }
    if (lhs.TryAs<Bool>() != nullptr && rhs.TryAs<Bool>() != nullptr) {
        return lhs.TryAs<Bool>()->GetValue() == rhs.TryAs<Bool>()->GetValue();
    }
    if (lhs.TryAs<ClassInstance>() != nullptr && rhs.TryAs<ClassInstance>() != nullptr) {
        if (lhs.TryAs<ClassInstance>()->HasMethod("__eq__"s,1)) {
            Closure args_table_;
            string argument_name = "rhs"s;
            args_table_[argument_name] = ObjectHolder::Share(*rhs);
            return IsTrue(
                lhs.TryAs<ClassInstance>()->Call("__eq__"s, { rhs }, context)
            );
        }
    }
    if (!lhs && !rhs) {
        return true;
    }
    throw std::runtime_error("Cannot compare objects for equality"s);
}

bool Less(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context) {
    if (lhs.TryAs<String>() != nullptr && rhs.TryAs<String>() != nullptr) {
        return lhs.TryAs<String>()->GetValue() < rhs.TryAs<String>()->GetValue();
    }
    if (lhs.TryAs<Number>() != nullptr && rhs.TryAs<Number>() != nullptr) {
        return lhs.TryAs<Number>()->GetValue() < rhs.TryAs<Number>()->GetValue();
    }
    if (lhs.TryAs<Bool>() != nullptr && rhs.TryAs<Bool>() != nullptr) {
        return lhs.TryAs<Bool>()->GetValue() < rhs.TryAs<Bool>()->GetValue();
    }
    if (lhs.TryAs<ClassInstance>() != nullptr && rhs.TryAs<ClassInstance>() != nullptr) {
        if (lhs.TryAs<ClassInstance>()->HasMethod("__lt__"s, 1)) {
            Closure args_table_;
            string argument_name = "rhs"s;
            args_table_[argument_name] = ObjectHolder::Share(*rhs);
            return IsTrue(
                lhs.TryAs<ClassInstance>()->Call("__lt__"s, { rhs }, context)
            );
        }
    }
    throw std::runtime_error("Cannot compare objects for equality"s);
}

bool NotEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context) {
    return !Equal(lhs, rhs, context);
}

bool Greater(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context) {
    return !Less(lhs, rhs, context) && NotEqual(lhs, rhs, context);
}

bool LessOrEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context) {
    return !Greater(lhs, rhs, context);
}

bool GreaterOrEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context) {
    return !Less(lhs, rhs, context);
}

}  // namespace runtime