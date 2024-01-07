#include "StackFrame.h"
#include "SymbolTable.h"

StackFrame::StackFrame() {
    locals = std::make_shared<SymbolTable>();
}
StackFrame::StackFrame(size_t returnAddress): returnAddress(returnAddress) {
    locals = std::make_shared<SymbolTable>();
}
