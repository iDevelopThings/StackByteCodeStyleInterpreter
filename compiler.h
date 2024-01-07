#pragma once

#include "Ast.h"
#include "BytecodeInstructions.h"
#include "Common.h"

class FunctionTable;

class Compiler
{
public:
    Shared<FunctionTable> functionTable = {};

    BytecodeInstructionSet instructions = {};

    Compiler();


    void compileProgram(const Shared<ProgramNode>& program);
    void compile(const Shared<AstNode>& node);

    void compileStruct(const Shared<StructNode>& node);

    void compileFunction(const Shared<FunctionNode>& node);

    void compileExpression(const Shared<ExprNode>& node);

    void compileStatement(const Shared<StmtNode>& node);

    void compileNodeList(const std::vector<Shared<AstNode>>& nodes);

    void compileBlock(const Shared<BlockNode>& node);

    void compileFunctionCall(const Shared<FunctionCallNode>& node);

    void compileBinaryOp(const Shared<BinaryOpNode>& node);

    void compileAssignment(const Shared<AssignmentNode>& node);

    void compileReturnStatement(const Shared<ReturnStatementNode>& node);

    void compileMemberAccess(const Shared<MemberAccessNode>& node);

    void compileVariableDeclaration(const Shared<VariableDeclarationNode>& node);

    // ... methods to compile other types of nodes ...
};
