#include "Interpreter.h"

#include "compiler.h"
#include "SymbolTable.h"
#include "Utils.h"


Interpreter::Interpreter(const Compiler& compiler):
    functionTable(compiler.functionTable) {
    bytecode = compiler.instructions;
}

Shared<SymbolTable> Interpreter::getTable() {
    auto& frame = callStack.back();
    return frame.locals;
}

void Interpreter::execute() {
    TIMED_FUNCTION();
    
    size_t pc = 0; // Program counter

    size_t startAddress;
    Shared<FunctionNode> func;
    if (!functionTable->resolve("main", func, startAddress)) {
        std::cerr << "Main function not found" << std::endl;
        return;
    }

    pc = startAddress;
    // callStack.push_back(StackFrame(startAddress));

    // Variable to capture the return value
    RuntimeValue returnValue;
    // Flag to indicate if a return value was captured
    bool returned = false;

    executeFunction("main", pc);
    
    if (pc == (startAddress + 1) && callStack.empty()) {
        // If we've returned to the start of `main` and the call stack is empty,
        // it means `main` has finished executing
        if (!stack.empty()) {
            // Assume the top of the stack holds `main`'s return value
            returnValue = stack.pop();
            returned = true;
        }
    }

    std::cout << "Execution complete. Address: " << pc << std::endl;
    if (returned) {
        std::cout << "Return value of main: " << returnValue << std::endl;
    } else {
        std::cout << "No return value." << std::endl;
    }
}


void Interpreter::executeInstruction(size_t& pc) {
    auto& instruction = bytecode[pc];

    switch (instruction.opcode) {
        case Opcode::LOAD_CONST:
            stack.push_back(instruction.operand);
            break;
        case Opcode::ADD:
            executeAdd();
            break;
        case Opcode::SUB:
            // Pop two operands
            // Subtract them
            // Push result
            break;
        case Opcode::MUL:
            // Pop two operands
            // Multiply them
            // Push result
            break;
        case Opcode::DIV:
            // Pop two operands
            // Divide them
            // Push result
            break;
        case Opcode::LOAD_VAR:
            executeLoadVar(instruction.operand.asString());
            break;
        case Opcode::STORE_VAR:
            executeStoreVar(instruction.operand.asString());
            break;
        case Opcode::LOAD_FIELD:
            executeLoadField(instruction.operand.asString());
            break;
        case Opcode::STORE_FIELD:
            executeStoreField(instruction.operand.asString());
            break;

        case Opcode::CALL_FUNC:
            executeFunction(instruction.operand.asString(), pc);
            break;

        case Opcode::RETURN:
        case Opcode::RETURN_VALUE:
            executeReturnValue(instruction, pc);
            break;
        default:
            throw std::runtime_error("Invalid opcode");
    }

    pc++;
}

void Interpreter::executeInstructionsUntilAddress(size_t& pc, size_t address) {
    while (pc < bytecode.size() && pc != address) {
        executeInstruction(pc);
    }
}

void Interpreter::executeAdd() {
    RuntimeValue right = stack.pop();
    RuntimeValue left = stack.pop();

    if (RuntimeValue::CanPerformOperation(left, ArithmeticOp::ADD, right)) {
        stack.emplace_back(left + right);
        return;
    }

    throw std::runtime_error("Type mismatch");
}

void Interpreter::executeLoadVar(const std::string& varName) {
    RuntimeValue& var = getTable()->resolve(varName);
    stack.push_back(var);
}

void Interpreter::executeStoreVar(const std::string& varName) {
    if (stack.empty()) {
        std::cerr << "Stack is empty" << std::endl;
        return;
    }

    auto table = getTable();

    RuntimeValue var = stack.pop();

    table->define(varName, var);
}

void Interpreter::executeLoadField(const std::string& fieldName) {
    auto& object = stack.back().asStruct();

    stack.push_back(object[fieldName]);
}

void Interpreter::executeStoreField(const std::string& fieldName) {
    auto fieldValue = stack.pop();
    auto& object = stack.back().asStruct();
    object[fieldName] = fieldValue;
}

void Interpreter::executeFunction(const std::string& funcName, size_t& pc) {
    size_t startAddress;
    Shared<FunctionNode> func;
    if (!functionTable->resolve(funcName, func, startAddress)) {
        std::cerr << "Function not found: " << funcName << std::endl;
        return;
    }

    size_t returnAddress = pc + 1;

    // std::cout << "Calling function: " << funcName << " at address: " << startAddress << std::endl;

    auto& frame = callStack.emplace_back(returnAddress);
    frame.locals = getTable()->createChild();

    for (auto it = func->parameters.rbegin(); it != func->parameters.rend(); ++it) {
        RuntimeValue arg = stack.pop();
        frame.locals->define(it->second, arg); // Parameter name and argument value
    }

    pc = startAddress; // Jump to the function's starting address

    while (pc < bytecode.size()) {
        executeInstruction(pc);

        if (bytecode[pc].opcode == Opcode::RETURN || bytecode[pc].opcode == Opcode::RETURN_VALUE) {
            break;
        }
    }
    
    // What do we actually do here to execute the function & handle responses?
    // executeInstructionsUntilAddress(pc, returnAddress);

    pc = callStack.back().returnAddress;
    callStack.pop_back();

    // std::cout << "Returned from function: " << funcName << " at address: " << startAddress << std::endl;
}

void Interpreter::executeReturnValue(const Instruction& InstructionType, size_t& pc) {
    pc = callStack.back().returnAddress;

    // std::cout << "Hit return" << std::endl;

    if (InstructionType.opcode == Opcode::RETURN_VALUE) {
        // RuntimeValue returnValue = stack.pop();
    }
}
