#pragma once

#include "Common.h"
#include "RuntimeValue.h"


enum class Opcode
{
    // Load constant
    LOAD_CONST,
    // Addition
    ADD,
    // Subtraction
    SUB,
    // Multiplication
    MUL,
    // Division
    DIV,
    // Modulus
    MOD,
    // Equality
    EQ,
    // Inequality
    NEQ,
    // Less than
    LT,
    // Less than or equal to
    LTE,
    // Greater than
    GT,
    // Greater than or equal to
    GTE,

    // Load variable
    LOAD_VAR,
    // Store variable
    STORE_VAR,
    // Store field
    STORE_FIELD,
    // Load field
    LOAD_FIELD,

    // Call function
    CALL_FUNC,
    // Push argument count
    // PUSH_ARG_COUNT,

    RETURN_VALUE,
    RETURN,
};

inline const char* to_string(Opcode e) {
    switch (e) {
        case Opcode::LOAD_CONST: return "LOAD_CONST";
        case Opcode::ADD: return "ADD";
        case Opcode::SUB: return "SUB";
        case Opcode::MUL: return "MUL";
        case Opcode::DIV: return "DIV";
        case Opcode::MOD: return "MOD";
        case Opcode::EQ: return "EQ";
        case Opcode::NEQ: return "NEQ";
        case Opcode::LT: return "LT";
        case Opcode::LTE: return "LTE";
        case Opcode::GT: return "GT";
        case Opcode::GTE: return "GTE";
        case Opcode::LOAD_VAR: return "LOAD_VAR";
        case Opcode::STORE_VAR: return "STORE_VAR";
        case Opcode::STORE_FIELD: return "STORE_FIELD";
        case Opcode::LOAD_FIELD: return "LOAD_FIELD";
        case Opcode::CALL_FUNC: return "CALL_FUNC";
        case Opcode::RETURN_VALUE: return "RETURN_VALUE";
        case Opcode::RETURN: return "RETURN";
        default: return "unknown";
    }
}


class Instruction
{
public:
    Opcode opcode;
    RuntimeValue operand;

    std::string debugContext;
};

class BytecodeInstructionSet : public std::vector<Instruction>
{
public:
    BytecodeInstructionSet();
    BytecodeInstructionSet(const std::vector<Instruction>& vec);

    // BytecodeInstructionSet(const BytecodeInstructionSet& other);
    // BytecodeInstructionSet(BytecodeInstructionSet&& other) noexcept;

    BytecodeInstructionSet& operator=(const BytecodeInstructionSet& other);

    BytecodeInstructionSet& operator=(BytecodeInstructionSet&& other) noexcept;

    BytecodeInstructionSet& operator+=(const BytecodeInstructionSet& rhs);

    // BytecodeInstructionSet operator+(const BytecodeInstructionSet& rhs) const;

    BytecodeInstructionSet& operator+=(const Instruction& rhs);

    // BytecodeInstructionSet operator+(const Instruction& rhs) const;

    BytecodeInstructionSet& operator+=(Instruction&& rhs);

    BytecodeInstructionSet& push(Opcode opcode, const RuntimeValue& operand = RuntimeValue(), const std::string& debugContext = "");
    
    void dump();
};
