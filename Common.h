#pragma once

#include <format>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include <utility>
#include <variant>
#include <unordered_map>
#include <string>


#include "Lexer.h"

// Custom `std::shared_ptr` for easier usage
template <typename T>
using Shared = std::shared_ptr<T>;

// template <typename T>
// struct Shared : std::shared_ptr<T>
// {
// using std::shared_ptr<T>::shared_ptr;

// template <typename U, std::enable_if_t<std::is_base_of_v<T, U>, int>  = 0>
// Shared(const std::shared_ptr<U>& other) : std::shared_ptr<T>(other) {
// }
// };


inline std::string formatStringWithLineNumbers(const std::string& str, size_t lineNumToHighlight, size_t charPosToHighlight) {
    std::istringstream stream(str);
    std::string line;
    std::vector<std::string> lines;
    int lineNum = 0;

    while (std::getline(stream, line)) {
        ++lineNum;
        std::ostringstream formattedLine;
        formattedLine << std::setw(5) << std::setfill(' ') << lineNum << " | " << line;
        lines.push_back(formattedLine.str());
    }

    if (lineNumToHighlight > 0 && lineNumToHighlight <= lines.size() &&
        charPosToHighlight >= 0 && charPosToHighlight < line.size()) {
        std::ostringstream caretLine;
        caretLine << std::setw(5 + 3 + charPosToHighlight) << std::setfill(' ') << '^';
        lines.insert(lines.begin() + lineNumToHighlight, caretLine.str());
    }

    std::ostringstream output;
    for (const auto& lineStr : lines) {
        output << lineStr << "\n";
    }

    return output.str();
}

inline std::string formatStringWithLineNumbers(const std::string& str, size_t charPosToHighlight) {
    std::istringstream stream(str);
    std::string line;
    std::vector<std::string> lines;
    int lineNum = 0;
    int remainingCharPos = charPosToHighlight;
    int charPosInLine = 0;

    while (std::getline(stream, line)) {
        ++lineNum;
        std::ostringstream formattedLine;
        formattedLine << std::setw(5) << std::setfill(' ') << lineNum << " | " << line;
        lines.push_back(formattedLine.str());

        if (remainingCharPos - static_cast<int>(line.size()) < 0) {
            charPosInLine = remainingCharPos;
            remainingCharPos = -1;
        } else {
            remainingCharPos -= line.size() + 1; // Include newline or EOF in the calculation
        }

        if (remainingCharPos == -1) {
            std::string caretLine(lines[lineNum - 1].length(), ' ');
            caretLine[5 + 3 + charPosInLine] = '^';
            lines[lineNum - 1] = lines[lineNum - 1] + "\n" + caretLine;
            break;
        }
    }

    std::ostringstream output;
    for (const auto& lineStr : lines) {
        output << lineStr << "\n";
    }

    return output.str();
}

template <typename T>
concept Streamable = requires(std::ostream os, T value)
{
    { os << value };
};
