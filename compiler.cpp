#include "Compiler.h"
#include "BytecodeInstructions.h"
#include "SymbolTable.h"
#include "Utils.h"

Compiler::Compiler() {
    functionTable = std::make_shared<FunctionTable>();
}


#define push_op(...) push_op_expanded(__VA_ARGS__, std::format("[{}:{}]: ", __FUNCTION__, __LINE__))
#define push_op_expanded(op, ...) instructions.push(Opcode::op, __VA_ARGS__)


// #define push_op(op, ...) instructions.push(Opcode::op, __VA_ARGS__, __FUNCTION__)

void Compiler::compileProgram(const Shared<ProgramNode>& program) {
    TIMED_FUNCTION();
    return compileNodeList(program->statements);
}

void Compiler::compile(const Shared<AstNode>& node) {
    if (const auto n = std::dynamic_pointer_cast<ProgramNode>(node)) {
        return compileNodeList(n->statements);
    }
    if (const auto n = std::dynamic_pointer_cast<StructNode>(node)) {
        return compileStruct(n);
    }
    if (const auto n = std::dynamic_pointer_cast<FunctionNode>(node)) {
        return compileFunction(n);
    }
    if (const auto n = std::dynamic_pointer_cast<ExprNode>(node)) {
        return compileExpression(n);
    }
    if (const auto n = std::dynamic_pointer_cast<StmtNode>(node)) {
        return compileStatement(n);
    }
    throw std::runtime_error("Unknown node type");
}

void Compiler::compileStruct(const Shared<StructNode>& node) {}

void Compiler::compileFunction(const Shared<FunctionNode>& node) {
    const size_t startAddress = instructions.size();

    // instructions.push(Opcode::PUSH_ARG_COUNT, static_cast<int>(node->parameters.size()));

    // Step 1: Set up parameters in local scope
    for (const auto& param : node->parameters) {
        push_op(STORE_VAR, param.second);
    }

    // Step 2: Compile the function body
    compileBlock(node->body);

    functionTable->define(node, startAddress);
}

void Compiler::compileExpression(const Shared<ExprNode>& node) {
    /*using ValueNodeTypes = std::tuple<float, int, bool, std::string>;
        // Helper lambda to process each type in the tuple
        std::apply([&]<typename... T>(T... args)
        {
            // Using fold expression to iterate over each type in the tuple
            ((bytecode = compileValueNodeHelper<T>(node), bytecode.empty()) || ...);
        }, ValueNodeTypes());

        if (!bytecode.empty()) {
            return bytecode;
        }*/

    if (const auto n = std::dynamic_pointer_cast<FloatNode>(node)) {
        push_op(LOAD_CONST, n->value);
        return;
    }
    if (const auto n = std::dynamic_pointer_cast<IntNode>(node)) {
        push_op(LOAD_CONST, n->value);
        return;
    }
    if (const auto n = std::dynamic_pointer_cast<BooleanNode>(node)) {
        push_op(LOAD_CONST, n->value);
        return;
    }
    if (const auto n = std::dynamic_pointer_cast<StringNode>(node)) {
        push_op(LOAD_CONST, n->value);
        return;
    }
    if (const auto n = std::dynamic_pointer_cast<VariableNode>(node)) {
        push_op(LOAD_VAR, n->identifier);
        return;
    }
    if (const auto n = std::dynamic_pointer_cast<BinaryOpNode>(node)) {
        return compileBinaryOp(n);
    }
    if (const auto n = std::dynamic_pointer_cast<FunctionCallNode>(node)) {
        return compileFunctionCall(n);
    }

    throw std::runtime_error("[compileExpression] Unknown/Unhandled node type: " + std::string(node->name));
}

void Compiler::compileStatement(const Shared<StmtNode>& node) {
    if (const auto n = std::dynamic_pointer_cast<BlockNode>(node)) {
        return compileBlock(n);
    }
    if (const auto n = std::dynamic_pointer_cast<AssignmentNode>(node)) {
        return compileAssignment(n);
    }
    if (const auto n = std::dynamic_pointer_cast<VariableDeclarationNode>(node)) {
        return compileVariableDeclaration(n);
    }
    if (const auto n = std::dynamic_pointer_cast<ReturnStatementNode>(node)) {
        return compileReturnStatement(n);
    }
    throw std::runtime_error("[compileStatement] Unknown/Unhandled node type: " + std::string(node->name));
}

void Compiler::compileNodeList(const std::vector<Shared<AstNode>>& nodes) {
    for (auto& stmt : nodes) {
        compile(stmt);
    }
}

void Compiler::compileBlock(const Shared<BlockNode>& node) {
    compileNodeList(node->statements);
}

void Compiler::compileFunctionCall(const Shared<FunctionCallNode>& node) {
    for (auto& arg : node->arguments) {
        compile(arg);
    }

    push_op(CALL_FUNC, node->functionName);
}

void Compiler::compileBinaryOp(const Shared<BinaryOpNode>& node) {
    compileExpression(node->left);
    compileExpression(node->right);

    switch (node->op) {
        case TokenType::Plus:
        case TokenType::PlusPlus:
            push_op(ADD, RuntimeValue());
            break;
        case TokenType::Minus:
        case TokenType::MinusMinus:
            push_op(SUB, RuntimeValue());
            break;
        case TokenType::Star:
            push_op(MUL, RuntimeValue());
            break;
        case TokenType::Slash:
            push_op(DIV, RuntimeValue());
            break;
        default:
            throw std::runtime_error("Invalid binary operator: " + std::string(to_string(node->op)));
    }
}

void Compiler::compileAssignment(const Shared<AssignmentNode>& node) {
    // Compile the right-hand side of the assignment
    if (node->rhs) {
        compileExpression(node->rhs);
    }

    // Compile the left-hand side (lhs) expression for member access
    if (const auto lhsMemberAccess = std::dynamic_pointer_cast<MemberAccessNode>(node->lhs)) {
        // Compile nested member access
        compileMemberAccess(lhsMemberAccess);
        push_op(STORE_FIELD, lhsMemberAccess->member);
    }

    /*if (auto lhsVar = std::dynamic_pointer_cast<VariableNode>(node->lhs)) {
            // Simple variable assignment
            bytecode.push_back(Instruction(Opcode::STORE_VAR, lhsVar->identifier));
        } else if (auto lhsMemberAccess = std::dynamic_pointer_cast<MemberAccessNode>(node->lhs)) {
            // Member access assignment
            bytecode.push_back(Instruction(Opcode::LOAD_VAR, lhsMemberAccess->object->identifier)); // Load object
            bytecode.push_back(Instruction(Opcode::STORE_FIELD, lhsMemberAccess->member)); // Store in field
        }
        
        bytecode.push_back(Instruction(Opcode::STORE_VAR, RuntimeValue(node->lhs)));*/
}

void Compiler::compileReturnStatement(const Shared<ReturnStatementNode>& node) {
    if (node->expression) {
        compileExpression(node->expression);
        push_op(RETURN_VALUE, RuntimeValue());
    } else {
        push_op(RETURN, RuntimeValue());
    }
}

void Compiler::compileMemberAccess(const Shared<MemberAccessNode>& node) {
    // Recursively compile nested member access
    if (const auto nestedMemberAccess = std::dynamic_pointer_cast<MemberAccessNode>(node->object)) {
        compileMemberAccess(nestedMemberAccess);
    } else if (const auto varNode = std::dynamic_pointer_cast<VariableNode>(node->object)) {
        push_op(LOAD_VAR, varNode->identifier);
    }

    push_op(LOAD_FIELD, node->member);
}

void Compiler::compileVariableDeclaration(const Shared<VariableDeclarationNode>& node) {}
