#include "Common.h"
#include "compiler.h"
#include "Interpreter.h"
#include "Parser.h"
#include "Utils.h"

int main() {
    TIMED_FUNCTION();

    std::string code = R"(
        int add(int a, int b) {
            return a + b;
        }

        int main() {
            return add(5, 6);
        }
    )";


    /*
    struct Something {
        int a;
        bool b;
        string c;
    }
    Something something;
    something.a = 5;
    something.b = true;
    something.c = "Hello";*/

    Shared<Lexer> lexer = std::make_shared<Lexer>(code);
    Shared<Parser> parser = std::make_shared<Parser>(lexer);

    Shared<ProgramNode> program = parser->parse();

    // parser->dump_info();
    // program->toString(std::cout);

    Compiler compiler;
    compiler.compileProgram(program);
    // compiler.instructions.dump();

    Interpreter interpreter(compiler);
    interpreter.execute();

    return 0;
}
