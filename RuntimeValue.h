#pragma once

#include <any>

#include "Common.h"
#include "RuntimeValue_Struct.h"
#include <iostream>


#define DEFINE_GETTER(type, isConst) \
template <typename T> \
type get() isConst { \
if (is<T>()) return std::get<T>(data); \
\
throw std::runtime_error("Type mismatch"); \
}

#define DEFINE_TYPE(defType, name, valueType) \
RuntimeValue(defType value) : type(valueType), data(value) {} \
defType name() const { return std::get<defType>(data); } \
defType& name() { return std::get<defType>(data); } \
/*operator defType& () { return name(); }*/ 

enum class ArithmeticOp
{
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    EQ,
    NEQ,
    LT,
    LTE,
    GT,
    GTE,
};

enum class ValueType
{
    None,
    Int,
    Float,
    String,
    Bool,
    // Pointer,
    Struct,
};

inline const char* to_string(ValueType e) {
    switch (e) {
        case ValueType::None: return "None";
        case ValueType::Int: return "Int";
        case ValueType::Float: return "Float";
        case ValueType::String: return "String";
        case ValueType::Bool: return "Bool";
        // case ValueType::Pointer: return "Pointer";
        case ValueType::Struct: return "Struct";
        default: return "unknown";
    }
}

// Forward declaration of template function getValueType
template <typename T>
constexpr ValueType getValueType() { return ValueType::None; }

#define GET_VALUE_TYPE(typeDef, valueType) \
template <> constexpr ValueType getValueType<typeDef>() { return valueType; }

GET_VALUE_TYPE(std::monostate, ValueType::None)
GET_VALUE_TYPE(int, ValueType::Int)
GET_VALUE_TYPE(float, ValueType::Float)
GET_VALUE_TYPE(std::string, ValueType::String)
GET_VALUE_TYPE(bool, ValueType::Bool)
// GET_VALUE_TYPE(void*, ValueType::Pointer)
GET_VALUE_TYPE(RuntimeStruct, ValueType::Struct)


#undef GET_VALUE_TYPE

class RuntimeValue
{
private:
    using ValueData = std::variant<
        // nullptr type
        std::monostate,
        int,
        float,
        std::string,
        bool,
        void*,
        RuntimeStruct
        //
    >;

public:
    ValueType type;
    ValueData data;

    RuntimeValue();
    RuntimeValue(std::string typeName, std::any inData);

    RuntimeValue(const RuntimeValue& other)        : type(other.type),          data(other.data) {}

    RuntimeValue(RuntimeValue&& other) noexcept        : type(other.type),          data(std::move(other.data)) {}

    RuntimeValue& operator=(const RuntimeValue& other) {
        if (this == &other)
            return *this;
        type = other.type;
        data = other.data;
        return *this;
    }

    RuntimeValue& operator=(RuntimeValue&& other) noexcept {
        if (this == &other)
            return *this;
        type = other.type;
        data = std::move(other.data);
        return *this;
    }

    DEFINE_TYPE(std::monostate, asNone, ValueType::None)
    DEFINE_TYPE(int, asInt, ValueType::Int)
    DEFINE_TYPE(float, asFloat, ValueType::Float)
    DEFINE_TYPE(std::string, asString, ValueType::String)
    DEFINE_TYPE(bool, asBool, ValueType::Bool)
    // DEFINE_TYPE(void*, asPointer, ValueType::Pointer)
    DEFINE_TYPE(RuntimeStruct, asStruct, ValueType::Struct)

    DEFINE_GETTER(T,)
    DEFINE_GETTER(T, const)

#pragma region "Type Tests"

    // template <typename T>
    // ValueType getValueType() const { return getValueType<T>(); }

    template <typename T>
    [[nodiscard]] bool is() const;

    template <typename T>
    static bool BothAre(const RuntimeValue& lhs, const RuntimeValue& rhs);

    template <typename T>
    bool BothAre(const RuntimeValue& rhs) const;

    // Handle (`Is<int>() && rhs.Is<float>()`) || (`Is<float>() && rhs.Is<int>()`)
    template <typename TLhs, typename TRhs>
    static bool BothAreEither(const RuntimeValue& lhs, const RuntimeValue& rhs);

#pragma endregion

#pragma region "Operator/Function Overloads"

    friend RuntimeValue operator+(const RuntimeValue& lhs, const RuntimeValue& rhs);
    friend RuntimeValue operator-(const RuntimeValue& lhs, const RuntimeValue& rhs);
    friend RuntimeValue operator*(const RuntimeValue& lhs, const RuntimeValue& rhs);
    friend RuntimeValue operator/(const RuntimeValue& lhs, const RuntimeValue& rhs);
    friend RuntimeValue operator%(const RuntimeValue& lhs, const RuntimeValue& rhs);
    friend RuntimeValue operator==(const RuntimeValue& lhs, const RuntimeValue& rhs);
    friend RuntimeValue operator!=(const RuntimeValue& lhs, const RuntimeValue& rhs);
    friend RuntimeValue operator<(const RuntimeValue& lhs, const RuntimeValue& rhs);
    friend RuntimeValue operator<=(const RuntimeValue& lhs, const RuntimeValue& rhs);
    friend RuntimeValue operator>(const RuntimeValue& lhs, const RuntimeValue& rhs);
    friend RuntimeValue operator>=(const RuntimeValue& lhs, const RuntimeValue& rhs);

    bool CanPerformOperation(ArithmeticOp op, const RuntimeValue& rhs) const;

    static bool CanPerformOperation(const RuntimeValue& lhs, ArithmeticOp op, const RuntimeValue& rhs);

#pragma endregion

    void print(std::ostream& os) const;

private:
    static RuntimeValue performModulusOperation(const RuntimeValue& lhs, const RuntimeValue& rhs);

    template <typename Op>
    static RuntimeValue performOperation(const RuntimeValue& lhs, const RuntimeValue& rhs, Op operation);
};

std::ostream& operator<<(std::ostream& os, const RuntimeValue& value);
