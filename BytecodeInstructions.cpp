#include "BytecodeInstructions.h"

BytecodeInstructionSet::BytecodeInstructionSet() = default;
BytecodeInstructionSet::BytecodeInstructionSet(const std::vector<Instruction>& vec): std::vector<Instruction>(vec) {}
// BytecodeInstructionSet::BytecodeInstructionSet(const BytecodeInstructionSet& other): std::vector<Instruction>(other) {}
// BytecodeInstructionSet::BytecodeInstructionSet(BytecodeInstructionSet&& other) noexcept: std::vector<Instruction>(std::move(other)) {}

BytecodeInstructionSet& BytecodeInstructionSet::operator=(const BytecodeInstructionSet& other) {
    if (this == &other)
        return *this;
    std::vector<Instruction>::operator =(other);
    return *this;
}

BytecodeInstructionSet& BytecodeInstructionSet::operator=(BytecodeInstructionSet&& other) noexcept {
    if (this == &other)
        return *this;
    std::vector<Instruction>::operator =(std::move(other));
    return *this;
}

BytecodeInstructionSet& BytecodeInstructionSet::operator+=(const BytecodeInstructionSet& rhs) {
    insert(end(), rhs.begin(), rhs.end());
    return *this;
}

// BytecodeInstructionSet BytecodeInstructionSet::operator+(const BytecodeInstructionSet& rhs) const {
//     BytecodeInstructionSet result(*this);
//     result += rhs;
//     return result;
// }

BytecodeInstructionSet& BytecodeInstructionSet::operator+=(const Instruction& rhs) {
    push_back(rhs);
    return *this;
}

// BytecodeInstructionSet BytecodeInstructionSet::operator+(const Instruction& rhs) const {
//     BytecodeInstructionSet result(*this);
//     result += rhs;
//     return result;
// }

BytecodeInstructionSet& BytecodeInstructionSet::operator+=(Instruction&& rhs) {
    push_back(std::move(rhs));
    return *this;
}

BytecodeInstructionSet& BytecodeInstructionSet::push(Opcode opcode, const RuntimeValue& operand, const std::string& debugContext) {
    push_back(Instruction(opcode, operand, debugContext));
    return *this;
}

void BytecodeInstructionSet::dump() {
    std::cout << "------------" << std::endl;
    for (auto& instruction : *this) {
        std::cout << instruction.debugContext << to_string(instruction.opcode) << " -> " << instruction.operand << std::endl;
    }
}
