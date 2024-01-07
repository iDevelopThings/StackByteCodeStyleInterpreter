#pragma once

#include "BytecodeInstructions.h"
#include "Common.h"
#include "StackFrame.h"

class Compiler;
class FunctionTable;

class OperandStack : public std::vector<RuntimeValue>
{
public:
    RuntimeValue pop() {
        auto value = back();
        pop_back();
        return value;
    }
};

class Interpreter
{
public:
    // Shared<SymbolTable> table;

    Shared<FunctionTable> functionTable;

    BytecodeInstructionSet bytecode;
    OperandStack stack;
    std::vector<StackFrame> callStack; // Call stack

    Interpreter(const Compiler& compiler);

    Shared<SymbolTable> getTable();

    void execute();

    
    void executeInstruction(size_t& pc);
    void executeInstructionsUntilAddress(size_t& pc, size_t address);

    void executeAdd();

    void executeLoadVar(const std::string& varName);

    void executeStoreVar(const std::string& varName);

    void executeLoadField(const std::string& fieldName);

    void executeStoreField(const std::string& fieldName);

    void executeFunction(const std::string& funcName, size_t& pc);
    void executeReturnValue(const Instruction& InstructionType, size_t& pc);
};
