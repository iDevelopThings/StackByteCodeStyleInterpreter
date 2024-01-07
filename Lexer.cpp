#include "Lexer.h"
#include "Common.h"

Lexer::Lexer(std::string input):
    input(std::move(input)) {
}

Token Lexer::nextToken() {
    // Skip whitespace
    /*while(isspace(current)) {
        advance();
    }*/
    while (pos < input.size() && isspace(input[pos])) {
        advance();
    }

    // char current = input[pos];

    if (pos == input.size())
        return tok(TokenType::EndOfFile, "");

    // Handle different token types


    // Handle identifiers and keywords
    if (isalpha(input[pos]) || input[pos] == '_') {
        const size_t start = pos;
        while (pos < input.size() && (isalnum(input[pos]) || input[pos] == '_'))
            advance();

        const std::string word = input.substr(start, pos - start);

        if (word == "struct") return tok(TokenType::StructKeyword, word, start);
        if (word == "int") return tok(TokenType::IntKeyword, word, start);
        if (word == "bool") return tok(TokenType::BoolKeyword, word, start);
        if (word == "string") return tok(TokenType::StringKeyword, word, start);
        if (word == "return") return tok(TokenType::ReturnKeyword, word, start);
        if (word == "true") return tok(TokenType::True, word, start);
        if (word == "false") return tok(TokenType::False, word, start);
        if (word == "if") return tok(TokenType::IfKeyword, word, start);
        if (word == "else") return tok(TokenType::ElseKeyword, word, start);
        // Add more keywords as needed

        return tok(TokenType::Identifier, word, start);
    }

    // Handle string literals
    if (input[pos] == '"') {
        const size_t start = ++pos; // Skip the opening quote
        while (pos < input.size() && input[pos] != '"') {
            advance();
        }
        if (pos == input.size())
            throw std::runtime_error("Unterminated string literal");
        const std::string value = input.substr(start, pos - start);
        advance(); // Skip the closing quote

        return tok(TokenType::StringLiteral, value, start);
    }

    // Example: handling numbers
    if (isdigit(input[pos])) {
        const size_t start = pos;
        while (pos < input.size() && isdigit(input[pos]))
            advance();

        return tok(TokenType::Number, input.substr(start, pos - start), start);
    }

    // Handle single character tokens
    {
        switch (input[pos]) {
        case ';':
            advance();
            return tok(TokenType::Semicolon, ";");
        case ',':
            advance();
            return tok(TokenType::Comma, ",");
        case '.':
            advance();
            return tok(TokenType::Dot, ".");
        case '*':
            advance();
            return tok(TokenType::Star, "*");
        case '/':
            advance();
            return tok(TokenType::Slash, "/");
        case '(':
            advance();
            return tok(TokenType::OpenParen, "(");
        case ')':
            advance();
            return tok(TokenType::CloseParen, ")");
        case '{':
            advance();
            return tok(TokenType::OpenBrace, "{");
        case '}':
            advance();
            return tok(TokenType::CloseBrace, "}");
        case '+':
            if (peek() == '+') {
                advance(2);
                return tok(TokenType::PlusPlus, "++");
            }
            advance();
            return tok(TokenType::Plus, "+");
        case '-':
            if (peek() == '-') {
                advance(2);
                return tok(TokenType::MinusMinus, "--");
            }
            advance();
            return tok(TokenType::Minus, "-");
        case '=':
            if (peek() == '=') {
                advance(2);
                return tok(TokenType::EqualsEquals, "==");
            }
            advance();
            return tok(TokenType::Equals, "=");
        default: break;
        // Add more as needed
        }
    }

    throw std::runtime_error(std::format(
        "Unexpected token: '{}' at position {} in input: {}",
        // Escape the current character if it is not printable
        isprint(input[pos]) ? input[pos] : '\\',
        pos,
        input
    ));
}

void Lexer::advance(int num) {
    if(pos + num < input.size()) {
        pos += num;
    } else {
        pos = input.size();
    }
}

char Lexer::peek() {
    if (pos + 1 < input.size()) {
        return input[pos + 1];
    }
    return '\0';
}

Token Lexer::peekToken() {
    // Save the current state
    const size_t savedPos = pos;
    // const char savedCurrent = current;

    // Get the next token
    Token nextToken = this->nextToken();

    // Restore the saved state
    pos = savedPos;
    // current = savedCurrent;

    return nextToken;
}

void Lexer::returnTo(const LexerState& lexerState) {
    pos = lexerState.to_pos;
    // current = lexerState.current;
}

Token Lexer::tok() const {
    return Token({pos, pos}, TokenType::EndOfFile, "");
}

Token Lexer::tok(TokenType type) const {
    return Token({pos, pos}, type, "");
}

Token Lexer::tok(TokenType type, std::string value) const {
    return Token({pos - value.size(), pos}, type, std::move(value));
}

Token Lexer::tok(TokenType type, std::string value, size_t fromPos) const {
    return Token({fromPos, pos}, type, std::move(value));
}
