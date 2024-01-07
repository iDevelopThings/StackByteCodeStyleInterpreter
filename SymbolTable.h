#pragma once

#include "Common.h"
#include "RuntimeValue.h"

class FunctionNode;

class SymbolTable : public std::enable_shared_from_this<SymbolTable>
{
    std::unordered_map<std::string, RuntimeValue> symbols;
    Shared<SymbolTable> parent;

public:
    SymbolTable(Shared<SymbolTable> parent = nullptr);

    Shared<SymbolTable> createChild();

    void define(const std::string& name, const RuntimeValue& value);
    
    RuntimeValue& resolve(const std::string& name);
};

class FunctionTable : public std::enable_shared_from_this<FunctionTable>
{
    Shared<FunctionTable> parent;
    std::unordered_map<std::string, Shared<FunctionNode>> defs = {};
    std::unordered_map<std::string, size_t> addresses = {};

public:
    FunctionTable(Shared<FunctionTable> parent = nullptr);

    Shared<FunctionTable> createChild();

    void define(const Shared<FunctionNode>& node, size_t address);

    bool resolve(const std::string& funcName, Shared<FunctionNode>& outNode, size_t& outAddress);
    bool resolve(const std::string& funcName, size_t& outAddress);
};
