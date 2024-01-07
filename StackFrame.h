#pragma once

#include "Common.h"

class SymbolTable;

class StackFrame
{
public:
    // Local variables
    Shared<SymbolTable> locals = nullptr;
    // Address to return to after function call
    size_t returnAddress = 0;

    StackFrame();

    StackFrame(size_t returnAddress);
};
