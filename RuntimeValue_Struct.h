#pragma once
#include <string>
#include <unordered_map>

class RuntimeValue;

class RuntimeStruct : public std::unordered_map<std::string, RuntimeValue>
{
public:
    void print(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, const RuntimeStruct& value);
