#include "Ast.h"

// os << "\tLHS: " << (lhs ? lhs->toString(os) : "nullptr") << std::endl;
// os << "\t{STRKEY}: "
// if({VARNAME}) {VARNAME->toString(os)} else os << "nullptr";
// os << std::endl;
#define NODE_TO_STRING_OPTIONAL(key, node) \
    os << "\t" << #key << ": "; \
    if(node) { \
        node->toString(os); \
    } else { \
        os << "nullptr"; \
    } \
    os << std::endl;

std::ostream& StructNode::toString(std::ostream& os) const {
    os << "StructNode: " << name << " { " << std::endl;
    os << "\tMembers: " << *this << std::endl;
    os << "}" << std::endl;

    return os;
}

std::ostream& ProgramNode::toString(std::ostream& os) const {
    os << "ProgramNode: " << name << " { " << std::endl;
    os << "\tStatements: " << std::endl;
    for (const auto& stmt : statements) {
        stmt->toString(os);
    }
    os << "\tStructs: " << std::endl;
    for (const auto& structNode : structs) {
        structNode.second->toString(os);
    }
    os << "\tFunctions: " << std::endl;
    for (const auto& functionNode : functions) {
        functionNode.second->toString(os);
    }
    os << "}" << std::endl;

    return os;
}

std::ostream& FunctionNode::toString(std::ostream& os) const {
    os << "FunctionNode: " << returnType << " " << name << std::endl;
    os << "\tParameters: " << std::endl;
    for (const auto& param : parameters) {
        os << "\t\t" << param.first << ": " << param.second << std::endl;
    }
    if (body) {
        body->toString(os);
    }
    os << " }" << std::endl;
    return os;
}

std::ostream& ExprNode::toString(std::ostream& os) const {
    os << "ExprNode: " << name;
    os << std::endl;
    return os;
}

std::ostream& VariableNode::toString(std::ostream& os) const {
    os << "VariableNode(" << name << ", " << identifier << ")";
    return os;
}

std::ostream& BinaryOpNode::toString(std::ostream& os) const {
    os << "BinaryOpNode(" << name << ", " << op << ") { " << std::endl;
    if (left) {
        left->toString(os);
    }
    if (right) {
        right->toString(os);
    }
    os << "}" << std::endl;
    return os;
}

std::ostream& StmtNode::toString(std::ostream& os) const {
    os << "StmtNode: " << name;
    os << std::endl;
    return os;
}

std::ostream& AssignmentNode::toString(std::ostream& os) const {
    os << "AssignmentNode(" << name << ") {" << std::endl;
    NODE_TO_STRING_OPTIONAL(LHS, lhs);
    NODE_TO_STRING_OPTIONAL(RHS, rhs);
    os << std::endl << "}" << std::endl;

    return os;
}

std::ostream& VariableDeclarationNode::toString(std::ostream& os) const {
    os << "VariableDeclarationNode(" << name << ") {" << std::endl;

    NODE_TO_STRING_OPTIONAL(Type, type);

    os << "}" << std::endl;

    return os;
}

std::ostream& FieldDeclarationNode::toString(std::ostream& os) const {
    os << "FieldDeclarationNode(";
    type->toString(os);
    os << ")" << std::endl;
    return os;
}

std::ostream& BlockNode::toString(std::ostream& os) const {
    os << "BlockNode(" << name << ") { " << std::endl;
    for (const auto& stmt : statements) {
        stmt->toString(os);
        os << std::endl;
    }
    os << "}" << std::endl;

    return os;
}

std::ostream& IfStatementNode::toString(std::ostream& os) const {
    os << "IfStatementNode(" << name << ")";
    os << " { ";
    os << "Condition: ";
    if (condition) {
        condition->toString(os);
    }
    os << ", ";
    os << "Then branch: ";
    if (thenBranch) {
        thenBranch->toString(os);
    }
    os << ", ";
    os << "Else branch: ";
    if (elseBranch) {
        elseBranch->toString(os);
    }
    os << " }";

    os << std::endl;
    return os;
}

std::ostream& MemberAccessNode::toString(std::ostream& os) const {
    os << "MemberAccessNode(" << name << ") { " << std::endl;
    NODE_TO_STRING_OPTIONAL(Variable, object);
    os << "\tMember: " << member << std::endl;
    os << "}" << std::endl;
    return os;
}

std::ostream& ReturnStatementNode::toString(std::ostream& os) const {
    os << "ReturnStatementNode(" << name << ")";
    os << " { ";
    os << "Expression: ";
    if (expression) {
        expression->toString(os);
    }
    os << " }";

    os << std::endl;
    return os;
}

std::ostream& FunctionCallNode::toString(std::ostream& os) const {
    os << "FunctionCallNode(" << name << ") { " << std::endl;
    os << "Function name: " << functionName << std::endl;
    os << "Arguments: ";

    for (const auto& arg : arguments) {
        arg->toString(os);
    }
    os << " }" << std::endl;

    return os;
}

std::ostream& TypeReferenceNode::toString(std::ostream& os) const {
    return os << "TypeReferenceNode(name=" << typeName << ", tokenType=" << to_string(tokenType) << ")" << std::endl;
}
