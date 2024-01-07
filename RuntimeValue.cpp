#include "RuntimeValue.h"

#include <algorithm>
#include <iostream>

RuntimeValue::RuntimeValue(): type(ValueType::None), data() {}

RuntimeValue::RuntimeValue(std::string typeName, std::any inData) {
    std::ranges::transform(typeName, typeName.begin(), ::tolower);

    if (typeName == "string") {
        type = ValueType::String;
        data = inData.has_value() ? std::any_cast<std::string>(inData) : std::string();
    } else if (typeName == "int") {
        type = ValueType::Int;
        data = inData.has_value() ? std::any_cast<int>(inData) : 0;
    } else if (typeName == "float") {
        type = ValueType::Float;
        data = inData.has_value() ? std::any_cast<float>(inData) : 0.0f;
    } else if (typeName == "bool") {
        type = ValueType::Bool;
        data = inData.has_value() ? std::any_cast<bool>(inData) : false;
    } else if (typeName == "struct") {
        type = ValueType::Struct;
        data = inData.has_value() ? std::any_cast<RuntimeStruct>(inData) : RuntimeStruct();
    } else {
        throw std::runtime_error("Invalid type name: " + typeName);
    }
}

template <typename T>
bool RuntimeValue::is() const { return type == getValueType<T>(); }

template <typename T>
bool RuntimeValue::BothAre(const RuntimeValue& lhs, const RuntimeValue& rhs) { return lhs.is<T>() && rhs.is<T>(); }

template <typename T>
bool RuntimeValue::BothAre(const RuntimeValue& rhs) const { return is<T>() && rhs.is<T>(); }

template <typename TLhs, typename TRhs>
bool RuntimeValue::BothAreEither(const RuntimeValue& lhs, const RuntimeValue& rhs) {
    if (lhs.is<TLhs>() && rhs.is<TRhs>())
        return true;
    if (lhs.is<TRhs>() && rhs.is<TLhs>())
        return true;

    return false;
}

template <typename Op>
RuntimeValue RuntimeValue::performOperation(const RuntimeValue& lhs, const RuntimeValue& rhs, Op operation) {
    if (BothAre<int>(lhs, rhs)) {
        return RuntimeValue(operation(lhs.get<int>(), rhs.get<int>()));
    }
    if (BothAre<float>(lhs, rhs)) {
        // compile time check to ensure that `Op` is not `std::modulus<>`

        if constexpr (std::is_same_v<Op, std::modulus<>>) {
            throw std::runtime_error("Cannot perform modulus operation on float types");
        }

        return RuntimeValue(operation(lhs.get<float>(), rhs.get<float>()));
    }
    /*if (BothAre<std::string>(lhs, rhs)) {
                if (operation == std::plus<>())
                    return RuntimeValue(lhs.get<std::string>() + rhs.get<std::string>());
                if (operation == std::minus<>())
                    throw std::runtime_error("Cannot subtract strings");
                if (operation == std::equal_to<>())
                    return RuntimeValue(lhs.get<std::string>() == rhs.get<std::string>());
                if (operation == std::not_equal_to<>())
                    return RuntimeValue(lhs.get<std::string>() != rhs.get<std::string>());
                if (operation == std::less<>())
                    return RuntimeValue(lhs.get<std::string>() < rhs.get<std::string>());
                if (operation == std::less_equal<>())
                    return RuntimeValue(lhs.get<std::string>() <= rhs.get<std::string>());
                if (operation == std::greater<>())
                    return RuntimeValue(lhs.get<std::string>() > rhs.get<std::string>());
                if (operation == std::greater_equal<>())
                    return RuntimeValue(lhs.get<std::string>() >= rhs.get<std::string>());
                if (operation == std::multiplies<>())
                    throw std::runtime_error("Cannot multiply strings");
                if (operation == std::divides<>())
                    throw std::runtime_error("Cannot divide strings");
                if (operation == std::modulus<>())
                    throw std::runtime_error("Cannot modulo strings");
            }*/
    // Handle type promotion so it's less annoying to work with
    if (BothAreEither<int, float>(lhs, rhs)) {
        if (lhs.is<int>() && rhs.is<float>())
            return RuntimeValue(operation(lhs.get<int>(), static_cast<int>(rhs.get<float>())));
        if (lhs.is<float>() && rhs.is<int>())
            return RuntimeValue(operation(static_cast<int>(lhs.get<float>()), rhs.get<int>()));
    }

    // Allow conversion of bool <-> int
    if (BothAreEither<bool, int>(lhs, rhs)) {
        if (lhs.is<bool>() && rhs.is<int>())
            return RuntimeValue(operation(lhs.get<bool>(), static_cast<bool>(rhs.get<int>())));
        if (lhs.is<int>() && rhs.is<bool>())
            return RuntimeValue(operation(static_cast<bool>(lhs.get<int>()), rhs.get<bool>()));
    }

    // Add more type combinations if needed
    throw std::runtime_error("Unsupported type combination for operation");
}

RuntimeValue operator+(const RuntimeValue& lhs, const RuntimeValue& rhs) { return RuntimeValue::performOperation(lhs, rhs, std::plus<>()); }

RuntimeValue operator-(const RuntimeValue& lhs, const RuntimeValue& rhs) { return RuntimeValue::performOperation(lhs, rhs, std::minus<>()); }

RuntimeValue operator*(const RuntimeValue& lhs, const RuntimeValue& rhs) { return RuntimeValue::performOperation(lhs, rhs, std::multiplies<>()); }

RuntimeValue operator/(const RuntimeValue& lhs, const RuntimeValue& rhs) { return RuntimeValue::performOperation(lhs, rhs, std::divides<>()); }

RuntimeValue operator%(const RuntimeValue& lhs, const RuntimeValue& rhs) { return RuntimeValue::performModulusOperation(lhs, rhs); }

RuntimeValue operator==(const RuntimeValue& lhs, const RuntimeValue& rhs) { return RuntimeValue::performOperation(lhs, rhs, std::equal_to<>()); }

RuntimeValue operator!=(const RuntimeValue& lhs, const RuntimeValue& rhs) { return RuntimeValue::performOperation(lhs, rhs, std::not_equal_to<>()); }

RuntimeValue operator<(const RuntimeValue& lhs, const RuntimeValue& rhs) { return RuntimeValue::performOperation(lhs, rhs, std::less<>()); }

RuntimeValue operator<=(const RuntimeValue& lhs, const RuntimeValue& rhs) { return RuntimeValue::performOperation(lhs, rhs, std::less_equal<>()); }

RuntimeValue operator>(const RuntimeValue& lhs, const RuntimeValue& rhs) { return RuntimeValue::performOperation(lhs, rhs, std::greater<>()); }

RuntimeValue operator>=(const RuntimeValue& lhs, const RuntimeValue& rhs) { return RuntimeValue::performOperation(lhs, rhs, std::greater_equal<>()); }

bool RuntimeValue::CanPerformOperation(ArithmeticOp op, const RuntimeValue& rhs) const { return CanPerformOperation(*this, op, rhs); }

bool RuntimeValue::CanPerformOperation(const RuntimeValue& lhs, ArithmeticOp op, const RuntimeValue& rhs) {
    const bool IntOrFloatCompatible = (BothAre<int>(lhs, rhs) || BothAre<float>(lhs, rhs)) || BothAreEither<int, float>(lhs, rhs);

    switch (op) {
        case ArithmeticOp::ADD:
        case ArithmeticOp::SUB:
        case ArithmeticOp::MUL:
        case ArithmeticOp::DIV:
        case ArithmeticOp::MOD:
            return IntOrFloatCompatible || BothAre<std::string>(lhs, rhs);
        case ArithmeticOp::EQ:
        case ArithmeticOp::NEQ:
        case ArithmeticOp::LT:
        case ArithmeticOp::LTE:
        case ArithmeticOp::GT:
        case ArithmeticOp::GTE:
            return IntOrFloatCompatible || BothAre<std::string>(lhs, rhs);
        default:
            return false;
    }
}

RuntimeValue RuntimeValue::performModulusOperation(const RuntimeValue& lhs, const RuntimeValue& rhs) {
    if (BothAre<int>(lhs, rhs)) {
        return RuntimeValue(std::modulus<>()(lhs.get<int>(), rhs.get<int>()));
    }
    throw std::runtime_error("Modulus operation is only valid for integers");
}


void RuntimeValue::print(std::ostream& os) const {
    os << "RuntimeValue -> ";
    os << "Type=" << to_string(type) << " ";
    switch (type) {
        case ValueType::Bool:
            os << "Value=" << std::boolalpha << get<bool>();
            break;
        case ValueType::Int:
            os << "Value=" << get<int>();
            break;
        case ValueType::Float:
            os << "Value=" << get<float>();
            break;
        case ValueType::String:
            os << "Value=" << get<std::string>();
            break;
        // case ValueType::Pointer:
        // os << "Value=Pointer(not displayed)";
        // break;
        case ValueType::Struct:
            os << "Value=Pointer(not displayed)";
            break;
        case ValueType::None:
            os << "Value=None";
            break;
    }
}

std::ostream& operator<<(std::ostream& os, const RuntimeValue& value) {
    value.print(os);
    return os;
}
