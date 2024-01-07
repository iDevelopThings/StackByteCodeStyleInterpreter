#include "SymbolTable.h"

#include "Ast.h"
#include "RuntimeValue.h"

SymbolTable::SymbolTable(Shared<SymbolTable> parent): parent(std::move(parent)) {}

Shared<SymbolTable> SymbolTable::createChild() {
    auto child = std::make_shared<SymbolTable>(shared_from_this());

    return child;
}

void SymbolTable::define(const std::string& name, const RuntimeValue& value) {
    symbols.insert_or_assign(name, value);
}

RuntimeValue& SymbolTable::resolve(const std::string& name) {
    if (symbols.contains(name)) {
        return symbols[name];
    }
    if (parent) {
        return parent->resolve(name);
    }
    throw std::runtime_error("[SymbolTable::resolve] Undefined variable: " + name);
}

FunctionTable::FunctionTable(Shared<FunctionTable> parent): parent(std::move(parent)) {}

Shared<FunctionTable> FunctionTable::createChild() {
    auto child = std::make_shared<FunctionTable>(shared_from_this());
    return child;
}

void FunctionTable::define(const Shared<FunctionNode>& node, size_t address) {
    defs.insert_or_assign(node->name, node);
    addresses.insert_or_assign(node->name, address);
}

bool FunctionTable::resolve(const std::string& funcName, Shared<FunctionNode>& outNode, size_t& outAddress) {
    if (!defs.contains(funcName))
        return false;

    outNode = defs[funcName];
    outAddress = addresses[funcName];

    return true;
}

bool FunctionTable::resolve(const std::string& funcName, size_t& outAddress) {
    Shared<FunctionNode> funcNode;
    return resolve(funcName, funcNode, outAddress);
}
