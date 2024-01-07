#pragma once
#include <algorithm>
#include <format>
#include <memory>

#include "Ast.h"
#include "Common.h"
#include "Utils.h"


class Parser
{
    Shared<Lexer> lexer;
    Token currentToken = {};

    std::vector<Shared<AstNode>> nodes;

    void advance() {
        currentToken = lexer->nextToken();
    }

    // Function to check if the current token matches the expected type
    bool match(TokenType type) {
        if (currentToken.type == type) {
            advance();
            return true;
        }
        return false;
    }

    // Function to ensure the current token is of the expected type
    void expect(TokenType type) {
        if (currentToken.type != type) {
            dump_info();
            throw std::runtime_error(
                std::format("Expected token type: {}, got: {}", to_string(type), to_string(currentToken.type))
            );
        }
        advance();
    }

    Token peek() { return lexer->peekToken(); }

    static bool isType(const Token& token) {
        // Function to determine if the token is a type keyword
        return token.type == TokenType::IntKeyword ||
            token.type == TokenType::BoolKeyword ||
            token.type == TokenType::StringKeyword;
    }

public:
    explicit Parser(Shared<Lexer> lexer) :
        lexer(std::move(lexer)) {
        advance();
    }

    Shared<ProgramNode> parse() {
        TIMED_FUNCTION();
        
        Shared<ProgramNode> program = std::make_shared<ProgramNode>();

        while (currentToken.type != TokenType::EndOfFile) {
            if (currentToken.type == TokenType::StructKeyword) {
                Shared<StructNode> structNode = parseStruct();
                program->statements.push_back(structNode);
                program->structs.emplace(structNode->name, structNode);
                continue;
            }

            if (isType(currentToken)) {
                Shared<AstNode> node = parseFunctionOrVariable();
                if (node != nullptr) {
                    if (auto functionNode = std::dynamic_pointer_cast<FunctionNode>(node)) {
                        program->functions.emplace(functionNode->name, functionNode);
                        program->statements.push_back(functionNode);
                        continue;
                    }

                    program->statements.push_back(node);
                    continue;
                }

                continue;
            }

            /*Shared<AstNode> node = parseTopLevel();
            if (node != nullptr) {
                nodes.push_back(node);
            }*/

            dump_info();

            throw std::runtime_error(std::format(
                "Unexpected token type: {} at position {}",
                to_string(currentToken.type),
                currentToken.lexerState.from_pos
            ));
        }

        return program;
    }

    void dump_info() {
        std::string start_region = lexer->input.substr(
            currentToken.lexerState.from_pos, -10
        );
        std::string end_region = lexer->input.substr(
            currentToken.lexerState.to_pos, 10
        );

        for (auto& ast_node : nodes) {
            ast_node->toString(std::cout);
        }

        std::cout << "Input:\n" << formatStringWithLineNumbers(
            lexer->input,
            currentToken.lexerState.to_pos
        ) << "\n\n";
    }

    /*Shared<AstNode> parseTopLevel() {
        if (isType(currentToken)) {
            return parseFunctionOrVariable();
        }

        dump_info();

        throw std::runtime_error(std::format(
            "Unexpected token type: {} at position {}",
            to_string(currentToken.type),
            currentToken.lexerState.from_pos
        ));
    }*/

    /**
     * struct Something {
     *   int a;
     *   bool b;
     *   string c;
     * }; 
     */
    Shared<StructNode> parseStruct() {
        expect(TokenType::StructKeyword);
        std::string structName = currentToken.value;
        expect(TokenType::Identifier);
        expect(TokenType::OpenBrace);

        Shared<StructNode> structNode = std::make_shared<StructNode>(structName);

        while (!match(TokenType::CloseBrace)) {
            // Parse struct members...
            // Example: int a;
            Token member = currentToken;
            advance(); // Advance over the type
            std::string memberName = currentToken.value;
            expect(TokenType::Identifier);
            expect(TokenType::Semicolon);

            structNode->addMember(memberName, member);
        }

        return structNode;
    }

    /**
     * int add(int a, int b) { 
     *     return a + b; 
     * } 
     */
    Shared<FunctionNode> parseFunction() {
        // Assume currentToken is the return type
        std::string returnType = currentToken.value;
        advance(); // Move to function name
        std::string functionName = currentToken.value;
        expect(TokenType::Identifier);

        expect(TokenType::OpenParen);
        std::vector<std::pair<std::string, std::string>> parameters;
        while (!match(TokenType::CloseParen)) {
            std::string paramType = currentToken.value;
            advance(); // Move to parameter name
            std::string paramName = currentToken.value;
            parameters.emplace_back(paramType, paramName);
            advance(); // Move to ',' or ')'
            if (currentToken.type == TokenType::Comma) {
                advance(); // Skip ','
            }
        }

        expect(TokenType::OpenBrace);
        Shared<BlockNode> body = std::make_shared<BlockNode>();
        while (!match(TokenType::CloseBrace)) {
            body->statements.push_back(parseStatement());
        }

        return std::make_shared<FunctionNode>(
            functionName, returnType, parameters, body
        );
    }

#pragma region "Statement Parsing"

    Shared<AstNode> parseStatement() {
        switch (currentToken.type) {
            case TokenType::Identifier:
                return parseAssignment();
            case TokenType::IfKeyword:
                return parseIfStatement();
            case TokenType::ReturnKeyword:
                return parseReturnStatement();
            // Add cases for other statement types like 'while', 'for', etc.
            default:
                throw std::runtime_error("Unexpected token in statement");
        }
    }

    Shared<ReturnStatementNode> parseReturnStatement() {
        expect(TokenType::ReturnKeyword);

        Shared<ExprNode> value = nullptr;
        if (currentToken.type != TokenType::Semicolon) {
            value = parseExpression();
        }

        expect(TokenType::Semicolon);

        return std::make_shared<ReturnStatementNode>(value);
    }

    Shared<AstNode> parseAssignment() {
        Shared<ExprNode> left = parseExpression(); // This will now handle member access

        Shared<ExprNode> value;
        // if we have an equals `SomeType someName = ...`
        if (currentToken.type == TokenType::Equals) {
            advance(); // Move to '='
            value = parseExpression();
        }

        // otherwise we have a `SomeType someName;`
        expect(TokenType::Semicolon);

        if (value == nullptr) {
            if (auto callNode = std::dynamic_pointer_cast<FunctionCallNode>(left)) {
                return callNode;
            }
        }


        return std::make_shared<AssignmentNode>(left, value);
    }

    /*Shared<AstNode> parseAssignment() {
        std::string varName = currentToken.value;
        advance(); // Move to '='
        expect(TokenType::Equals);
        Shared<ExprNode> value = parseExpression();
        expect(TokenType::Semicolon);
        return std::make_shared<AssignmentNode>(varName, value);
    }*/

    Shared<AstNode> parseFunctionOrVariable() {
        const Token startToken = currentToken;

        // Save the type and then advance to the next token, which should be the identifier
        Token type = currentToken;
        advance(); // Now currentToken is the identifier

        std::string identifier = currentToken.value;
        expect(TokenType::Identifier);

        // If next token is '(', it's a function
        if (currentToken.type == TokenType::OpenParen) {
            // It's a function. Roll back to type token and then parse the function
            currentToken = startToken;
            lexer->returnTo(startToken.lexerState);

            return parseFunction();
        }

        // It's a variable declaration
        advance(); // Move to ';'
        // expect(TokenType::Semicolon);
        return std::make_shared<VariableDeclarationNode>(identifier, type);
    }

    Shared<AstNode> parseIfStatement() {
        expect(TokenType::IfKeyword);
        expect(TokenType::OpenParen);
        Shared<ExprNode> condition = parseExpression();
        expect(TokenType::CloseParen);

        expect(TokenType::OpenBrace);
        std::vector<Shared<AstNode>> ifBody;
        while (!match(TokenType::CloseBrace)) {
            ifBody.push_back(parseStatement());
        }

        Shared<BlockNode> elseBody = nullptr;
        if (match(TokenType::ElseKeyword)) {
            expect(TokenType::OpenBrace);
            std::vector<Shared<AstNode>> elseBodyStatements;
            while (!match(TokenType::CloseBrace)) {
                elseBodyStatements.push_back(parseStatement());
            }
            elseBody = std::make_shared<BlockNode>(elseBodyStatements);
        }

        return std::make_shared<IfStatementNode>(
            condition,
            std::make_shared<BlockNode>(ifBody),
            elseBody
        );
    }

#pragma endregion

#pragma region "Expression Parsing"

    Shared<ExprNode> parseExpression() {
        return parseBinaryExpression(0);
    }

    Shared<ExprNode> parseBinaryExpression(int minPrecedence) {
        Shared<ExprNode> left = parsePrimaryExpression();

        while (true) {
            Token thisToken = currentToken;
            int precedence = getPrecedence(thisToken.type);
            if (precedence < minPrecedence)
                break;

            // Check if the current token is a semicolon or other ending token
            if (thisToken.type == TokenType::Semicolon ||
                thisToken.type == TokenType::CloseParen ||
                thisToken.type == TokenType::CloseBrace ||
                thisToken.type == TokenType::Comma ||
                thisToken.type == TokenType::EndOfFile
            ) {
                break;
            }

            TokenType op = thisToken.type;
            advance();

            if (currentToken.type == TokenType::Semicolon ||
                currentToken.type == TokenType::CloseParen ||
                currentToken.type == TokenType::CloseBrace ||
                currentToken.type == TokenType::Comma ||
                currentToken.type == TokenType::EndOfFile
            ) {
                break;
            }

            Shared<ExprNode> right = parseBinaryExpression(precedence + 1);

            left = std::make_shared<BinaryOpNode>(left, op, right);
        }

        return left;
    }

    Shared<ExprNode> parsePrimaryExpression() {
        switch (currentToken.type) {
            case TokenType::Number:
                return parseNumber();
            case TokenType::True:
            case TokenType::False:
                return parseBoolean();
            case TokenType::StringLiteral:
                return parseString();
            case TokenType::Identifier: {
                Token startToken = currentToken;
                std::string identifier = currentToken.value;
                advance(); // Move past the identifier

                // Check if this is a function call
                if (currentToken.type == TokenType::OpenParen) {
                    advance(); // Move past the '('
                    std::vector<Shared<ExprNode>> arguments;

                    // Parse function arguments
                    while (currentToken.type != TokenType::CloseParen) {
                        arguments.push_back(parseExpression());
                        if (currentToken.type == TokenType::Comma) {
                            advance(); // Skip the comma
                        }
                    }

                    expect(TokenType::CloseParen); // Expect the closing ')'

                    return std::make_shared<FunctionCallNode>(identifier, arguments);
                }

                if (currentToken.type == TokenType::Dot) {
                    lexer->returnTo(startToken.lexerState);
                    currentToken = startToken;

                    Shared<ExprNode> left = parseVariable();
                    while (currentToken.type == TokenType::Dot) {
                        advance(); // Move past the dot
                        std::string member = currentToken.value;
                        expect(TokenType::Identifier);

                        left = std::make_shared<MemberAccessNode>(left, member);
                    }

                    return left;
                }

                // It's just a variable
                return std::make_shared<VariableNode>(identifier);
            }
            // Add cases for other types of primary expressions (e.g., parentheses, function calls)
            default:
                dump_info();
                throw std::runtime_error(
                    std::format("Unexpected token in expression: {}", to_string(currentToken.type))
                );
        }
    }

    Shared<IntNode> parseNumber() {
        Shared<IntNode> node = std::make_shared<IntNode>(
            std::stoi(currentToken.value)
        );
        advance();
        return node;
    }

    Shared<BooleanNode> parseBoolean() {
        // convert `currentToken.value` to lowercase
        std::string value = currentToken.value;
        std::ranges::transform(value, value.begin(), [](unsigned char c) { return std::tolower(c); });

        Shared<BooleanNode> node = std::make_shared<BooleanNode>(value == "true");
        advance();
        return node;
    }

    Shared<StringNode> parseString() {
        Shared<StringNode> node = std::make_shared<StringNode>(currentToken.value);
        advance();
        return node;
    }

    Shared<ExprNode> parseVariable() {
        Shared<ExprNode> node = std::make_shared<VariableNode>(currentToken.value);
        advance();
        return node;
    }

    static int getPrecedence(TokenType type) {
        // Return the operator precedence
        switch (type) {
            case TokenType::Plus:
            case TokenType::Minus:
                return 1;
            case TokenType::Star:
            case TokenType::Slash:
                return 2;
            // Add more operators and their precedence as needed
            default:
                return 0;
        }
    }

#pragma endregion
};
