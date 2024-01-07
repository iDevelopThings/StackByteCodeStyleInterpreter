cmake_minimum_required(VERSION 3.27)
project(ScriptingLang)

set(CMAKE_CXX_STANDARD 23)

add_executable(ScriptingLang main.cpp
        Lexer.cpp
        Lexer.h
        Parser.cpp
        Parser.h
        Common.h
        Ast.cpp
        Ast.h
        Interpreter.cpp
        Interpreter.h
        StackFrame.cpp
        StackFrame.h
        SymbolTable.cpp
        SymbolTable.h
        RuntimeValue.cpp
        RuntimeValue.h
        RuntimeValue_Struct.h
        BytecodeInstructions.cpp
        BytecodeInstructions.h
        compiler.h
        compiler.cpp
        RuntimeValue_Struct.cpp
        Utils.cpp
        Utils.h
)
