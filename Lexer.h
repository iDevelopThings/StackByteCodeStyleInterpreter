#pragma once
#include <stdexcept>
#include <string>

class Lexer;
class unexpected_token;

enum class TokenType
{
    EndOfFile,
    IntKeyword,
    BoolKeyword,
    StringKeyword,
    StructKeyword,
    IfKeyword,
    ElseKeyword,
    Identifier,
    Number,
    StringLiteral,
    Semicolon,
    Comma,
    Dot,
    OpenParen,
    CloseParen,
    OpenBrace,
    CloseBrace,
    Star,
    Slash,
    ReturnKeyword,
    Equals,
    EqualsEquals,
    Plus,
    PlusPlus,
    Minus,
    MinusMinus,
    True,
    False
    // Add more as needed
};

inline const char* to_string(TokenType e) {
    switch (e) {
    case TokenType::EndOfFile: return "EndOfFile";
    case TokenType::IntKeyword: return "IntKeyword";
    case TokenType::BoolKeyword: return "BoolKeyword";
    case TokenType::StringKeyword: return "StringKeyword";
    case TokenType::StructKeyword: return "StructKeyword";
    case TokenType::IfKeyword: return "IfKeyword";
    case TokenType::ElseKeyword: return "ElseKeyword";
    case TokenType::Identifier: return "Identifier";
    case TokenType::Number: return "Number";
    case TokenType::StringLiteral: return "StringLiteral";
    case TokenType::Semicolon: return "Semicolon";
    case TokenType::Comma: return "Comma";
    case TokenType::Dot: return "Dot";
    case TokenType::OpenParen: return "OpenParen";
    case TokenType::CloseParen: return "CloseParen";
    case TokenType::OpenBrace: return "OpenBrace";
    case TokenType::CloseBrace: return "CloseBrace";
    case TokenType::Star: return "Star";
    case TokenType::Slash: return "Slash";
    case TokenType::ReturnKeyword: return "ReturnKeyword";
    case TokenType::Equals: return "Equals";
    case TokenType::EqualsEquals: return "EqualsEquals";
    case TokenType::Plus: return "Plus";
    case TokenType::PlusPlus: return "PlusPlus";
    case TokenType::Minus: return "Minus";
    case TokenType::MinusMinus: return "MinusMinus";
    case TokenType::True: return "True";
    case TokenType::False: return "False";
    default: return "unknown";
    }
}


struct LexerState
{
    size_t from_pos = 0;
    size_t to_pos = 0;
    // char current = '\0';
};

struct Token
{
    LexerState lexerState = {};

    TokenType type = TokenType::EndOfFile;
    std::string value;

    Token() = default;

    Token(const TokenType type, std::string value) :
        type(type),
        value(std::move(value)) {
    }

    Token(const LexerState& lexerState, const TokenType type, std::string value) :
        lexerState(lexerState),
        type(type),
        value(std::move(value)) {
    }
};

class Lexer
{
    friend class Parser;

    std::string input;
    size_t pos = 0;
    // char current = '\0';

public:
    explicit Lexer(std::string input);

    Token nextToken();

    void advance(int num = 1);

    char peek();

    Token peekToken();

    void returnTo(const LexerState& lexerState);

    [[nodiscard]] Token tok() const;

    [[nodiscard]] Token tok(TokenType type) const;

    [[nodiscard]] Token tok(TokenType type, std::string value) const;

    [[nodiscard]] Token tok(TokenType type, std::string value, size_t fromPos) const;
};
