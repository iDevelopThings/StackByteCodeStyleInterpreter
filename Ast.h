#pragma once

#include "Common.h"

class StructNode;
class FunctionNode;
class BlockNode;

class AstNode : public std::enable_shared_from_this<AstNode>
{
public:
    std::string name;

    AstNode() = default;

    explicit AstNode(std::string name) :
        name(std::move(name)) {}

    virtual ~AstNode() = default;

    virtual std::ostream& toString(std::ostream& os) const = 0;
};

class ProgramNode : public AstNode
{
public:
    std::vector<Shared<AstNode>> statements;
    
    std::unordered_map<std::string, Shared<FunctionNode>> functions;
    std::unordered_map<std::string, Shared<StructNode>> structs;

    explicit ProgramNode(std::vector<Shared<AstNode>> statements = {}):
        AstNode("Program"),
        statements(std::move(statements)) {}

    std::ostream& toString(std::ostream& os) const override;
};

class FunctionNode : public AstNode
{
public:
    std::string returnType;
    std::vector<std::pair<std::string, std::string>> parameters;
    Shared<BlockNode> body;

    FunctionNode() = default;

    explicit FunctionNode(std::string name) :
        AstNode(std::move(name)) {}

    explicit FunctionNode(
        std::string name,
        std::string returnType,
        std::vector<std::pair<std::string, std::string>> parameters,
        Shared<BlockNode> body
    ) :
        AstNode(std::move(name)),
        returnType(std::move(returnType)),
        parameters(std::move(parameters)),
        body(std::move(body)) {}

    std::ostream& toString(std::ostream& os) const override;
};

#pragma region "Expression Nodes"

class ExprNode : public AstNode
{
public:
    ExprNode() = default;

    explicit ExprNode(std::string name) : AstNode(std::move(name)) {}

    std::ostream& toString(std::ostream& os) const override;
};

class VariableNode : public ExprNode
{
public:
    std::string identifier;

    explicit VariableNode(std::string identifier) :
        ExprNode("Variable"),
        identifier(std::move(identifier)) {}

    std::ostream& toString(std::ostream& os) const override;
};

class BinaryOpNode : public ExprNode
{
public:
    Shared<ExprNode> left;
    Shared<ExprNode> right;
    TokenType op;

    BinaryOpNode(Shared<ExprNode> left, TokenType op, Shared<ExprNode> right):
        ExprNode("BinaryOp"),
        left(std::move(left)),
        right(std::move(right)),
        op(op) {}

    std::ostream& toString(std::ostream& os) const override;
};

#pragma endregion

#pragma region "Statement Nodes"

class StmtNode : public AstNode
{
public:
    StmtNode() = default;

    explicit StmtNode(std::string name) : AstNode(std::move(name)) {}

    std::ostream& toString(std::ostream& os) const override;
};

class AssignmentNode : public StmtNode
{
public:
    Shared<ExprNode> lhs;
    Shared<ExprNode> rhs;

    AssignmentNode(Shared<ExprNode> lhs, Shared<ExprNode> rhs) :
        StmtNode("Assignment"),
        lhs(std::move(lhs)),
        rhs(std::move(rhs)) {}

    std::ostream& toString(std::ostream& os) const override;
};

class BlockNode : public StmtNode
{
public:
    std::vector<Shared<AstNode>> statements;

    explicit BlockNode(std::vector<Shared<AstNode>> statements = {}):
        StmtNode("Block"),
        statements(std::move(statements)) {}

    std::ostream& toString(std::ostream& os) const override;
};

class IfStatementNode : public StmtNode
{
public:
    Shared<ExprNode> condition;
    Shared<BlockNode> thenBranch;
    Shared<BlockNode> elseBranch;

    IfStatementNode(
        Shared<ExprNode> condition,
        Shared<BlockNode> thenBranch,
        Shared<BlockNode> elseBranch = {}
    ) :
        StmtNode("IfStatement"),
        condition(std::move(condition)),
        thenBranch(std::move(thenBranch)),
        elseBranch(std::move(elseBranch)) {}

    std::ostream& toString(std::ostream& os) const override;
};

class MemberAccessNode : public ExprNode
{
public:
    Shared<ExprNode> object;
    std::string member;

    MemberAccessNode(Shared<ExprNode> object, std::string member)
        : ExprNode("MemberAccess"),
          object(std::move(object)),
          member(std::move(member)) {}

    std::ostream& toString(std::ostream& os) const override;
};

class ReturnStatementNode : public StmtNode
{
public:
    Shared<ExprNode> expression;

    explicit ReturnStatementNode(Shared<ExprNode> expression = {}) :
        StmtNode("ReturnStatement"),
        expression(std::move(expression)) {}

    std::ostream& toString(std::ostream& os) const override;
};

class FunctionCallNode : public ExprNode
{
public:
    std::string functionName;
    std::vector<Shared<ExprNode>> arguments;

    FunctionCallNode(std::string functionName, std::vector<Shared<ExprNode>> arguments)
        : ExprNode("FunctionCall"),
          functionName(std::move(functionName)),
          arguments(std::move(arguments)) {}

    std::ostream& toString(std::ostream& os) const override;
};

#pragma endregion

#pragma region "Declaration Nodes"

class TypeReferenceNode : public AstNode
{
public:
    std::string typeName;
    TokenType tokenType;

    explicit TypeReferenceNode(std::string typeName, const TokenType tokenType):
        AstNode("TypeReference"),
        typeName(std::move(typeName)),
        tokenType(tokenType) {}

    explicit TypeReferenceNode(Token token):
        AstNode("TypeReference"),
        typeName(std::move(token.value)),
        tokenType(token.type) {}

    std::ostream& toString(std::ostream& os) const override;
};

class VariableDeclarationNode : public StmtNode
{
public:
    std::string name;
    Shared<TypeReferenceNode> type;

    VariableDeclarationNode(std::string name, Shared<TypeReferenceNode> type):
        StmtNode("VariableDeclaration"),
        name(std::move(name)),
        type(std::move(type)) {}

    VariableDeclarationNode(std::string name, Token token):
        StmtNode("VariableDeclaration"),
        name(std::move(name)) {
        type = std::make_shared<TypeReferenceNode>(token);
    }

    VariableDeclarationNode(std::string name, std::string typeName, const TokenType tokenType):
        StmtNode("VariableDeclaration"),
        name(std::move(name)) {
        type = std::make_shared<TypeReferenceNode>(std::move(typeName), tokenType);
    }

    std::ostream& toString(std::ostream& os) const override;
};

class FieldDeclarationNode : public StmtNode
{
public:
    std::string name;
    Shared<TypeReferenceNode> type;

    FieldDeclarationNode(std::string name, Shared<TypeReferenceNode> type):
        StmtNode("FieldDeclaration"),
        name(std::move(name)),
        type(std::move(type)) {}

    FieldDeclarationNode(std::string name, Token token):
        StmtNode("FieldDeclaration"),
        name(std::move(name)),
        type(std::make_shared<TypeReferenceNode>(token)) {}

    FieldDeclarationNode(std::string name, std::string typeName, const TokenType tokenType):
        StmtNode("FieldDeclaration"),
        name(std::move(name)),
        type(std::make_shared<TypeReferenceNode>(std::move(typeName), tokenType)) {}

    std::ostream& toString(std::ostream& os) const override;
};

class IHasFields
{
public:
    std::unordered_map<std::string, Shared<FieldDeclarationNode>> fields;

    Shared<FieldDeclarationNode> addMember(const std::string& name, const Shared<TypeReferenceNode>& type) {
        Shared<FieldDeclarationNode> field = std::make_shared<FieldDeclarationNode>(name, type);
        fields.emplace(name, field);
        return field;
    }

    Shared<FieldDeclarationNode> addMember(const std::string& name, const Token& token) {
        Shared<FieldDeclarationNode> field = std::make_shared<FieldDeclarationNode>(name, token);
        fields.emplace(name, field);
        return field;
    }

    Shared<FieldDeclarationNode> addMember(const std::string& name, const std::string& typeName, const TokenType tokenType) {
        Shared<FieldDeclarationNode> field = std::make_shared<FieldDeclarationNode>(name, typeName, tokenType);
        fields.emplace(name, field);
        return field;
    }

    friend std::ostream& operator<<(std::ostream& os, const IHasFields& node) {
        for (const auto& field : node.fields) {
            os << "\t\t" << field.first << ": ";
            if (field.second->type) {
                os << field.second->type->typeName;
            }
            os << std::endl;
        }
        return os;
    }
};

class StructNode : public AstNode, public IHasFields
{
public:
    StructNode() = default;

    explicit StructNode(std::string name) :
        AstNode(std::move(name)) {}

    std::ostream& toString(std::ostream& os) const override;
};

#pragma endregion

inline std::ostream& operator<<(std::ostream& os, const TokenType& tokenType) {
    os << to_string(tokenType);
    return os;
}


#pragma region "Value Nodes"

/*template <typename T>
class ValueNode : public ExprNode
{
public:
    T value;

    // General constructor for all types except std::string
    explicit ValueNode(T value) : ExprNode("ValueNode"), value(std::move(value)) {}
    // Overloaded constructor for int
    ValueNode(int value) : ExprNode("ValueNode(int)"), value(value) {}
    // Overloaded constructor for float
    ValueNode(float value) : ExprNode("ValueNode(float)"), value(value) {}
    // Overloaded constructor for bool
    ValueNode(bool value) : ExprNode("ValueNode(bool)"), value(value) {}
    // Overloaded constructor for std::string
    ValueNode(std::string value) : ExprNode("ValueNode(string)"), value(std::move(value)) {}

    std::ostream& toString(std::ostream& os) const override {
        os << name << "(" << value << ")" << std::endl;
        return os;
    }
};*/

/*template <typename T>
class BaseNumberNode : public ExprNode
{
public:
    
    // Enable this constructor only for int and float types
    template <typename U = T, typename = std::enable_if_t<std::is_same_v<U, int> || std::is_same_v<U, float>>>
    explicit BaseNumberNode(U value) : ValueNode<T>(value) {}
};*/

class FloatNode : public ExprNode
{
public:
    float value;
    explicit FloatNode(const float value) : ExprNode("FloatNode"), value(value) {}
};

class IntNode : public ExprNode
{
public:
    int value;
    explicit IntNode(const int value) : ExprNode("IntNode"), value(value) {}
};


class BooleanNode : public ExprNode
{
public:
    bool value;
    explicit BooleanNode(const bool value) : ExprNode("BooleanNode"), value(value) {}
};

class StringNode : public ExprNode
{
public:
    std::string value;
    explicit StringNode(const std::string& value) : ExprNode("StringNode"), value(std::move(value)) {}
};

/*template <typename T>
T getValueNodeValue(const Shared<ExprNode>& node) {
    if (const auto n = std::dynamic_pointer_cast<ValueNode<T>>(node)) {
        return n->value;
    }
    throw std::runtime_error("Type mismatch");
}

template <>
constexpr int getValueNodeValue<int>(const Shared<ExprNode>& node) {
    if (const auto n = std::dynamic_pointer_cast<IntNode>(node)) {
        return n->value;
    }
    throw std::runtime_error("Type mismatch");
}

template <>
constexpr float getValueNodeValue<float>(const Shared<ExprNode>& node) {
    if (const auto n = std::dynamic_pointer_cast<FloatNode>(node)) {
        return n->value;
    }
    throw std::runtime_error("Type mismatch");
}

template <>
constexpr bool getValueNodeValue<bool>(const Shared<ExprNode>& node) {
    if (const auto n = std::dynamic_pointer_cast<BooleanNode>(node)) {
        return n->value;
    }
    throw std::runtime_error("Type mismatch");
}

template <>
constexpr std::string getValueNodeValue<std::string>(const Shared<ExprNode>& node) {
    if (const auto n = std::dynamic_pointer_cast<StringNode>(node)) {
        return n->value;
    }
    throw std::runtime_error("Type mismatch");
}*/


#pragma endregion
