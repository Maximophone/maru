#include "catch.hpp"
#include <string>
#include <vector>
#include "parser.hpp"

using namespace std;

Program* get_program(string input, int wanted_size){
    Lexer* l = new Lexer(input);
    Parser* p = new Parser(l);
    Program* program = p->parse_program();

    REQUIRE(program != 0);
    REQUIRE(program->statements.size() == wanted_size);

    return program;
};

void test_let_statement(Statement* statement, string exp_ident){
    REQUIRE(statement->token_literal()=="let");
    LetStatement* let_statement = (LetStatement*) statement;
    REQUIRE(let_statement->name->value == exp_ident);
    REQUIRE(let_statement->name->token_literal() == exp_ident);
};

TEST_CASE("test let statements"){
    string input = "let x = 5;"
    "let y = 10;"
    "let foobar = 838383;";

    Program* program = get_program(input, 3);

    struct test{string exp_ident;};
    vector<test> tests = {
        {"x"},
        {"y"},
        {"foobar"},
    };

    int i = 0;
    for(test t : tests){
        Statement* statement = program->statements[i];
        test_let_statement(statement, t.exp_ident);
        i++;
    }
}

