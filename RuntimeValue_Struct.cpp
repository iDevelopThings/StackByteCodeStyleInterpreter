#include "RuntimeValue_Struct.h"
#include "RuntimeValue.h"
#include <iostream>


void RuntimeStruct::print(std::ostream& os) const {
    os << "RuntimeStruct: {";
    for (auto& kv : *this) {
        os << kv.first << ": " << kv.second << ", ";
    }
    os << "}";
}

std::ostream& operator<<(std::ostream& os, const RuntimeStruct& value) {
    value.print(os);
    return os;
}
