#include "catch.hpp"
#include <string>
#include <vector>
#include "parser.hpp"

using namespace std;

void check_parser_errors(Parser*);
Program* get_program(string, int);
void test_let_statement(Statement*, string);
void test_return_statement(Statement*);
void check_parser_errors(Parser*);

Program* get_program(string input, int wanted_size){
    Lexer* l = new Lexer(input);
    Parser* p = new Parser(l);
    Program* program = p->parse_program();

    check_parser_errors(p);
    REQUIRE(program != 0);
    REQUIRE(program->statements.size() == wanted_size);

    return program;
};

void test_let_statement(Statement* statement, string exp_ident){
    REQUIRE(dynamic_cast<LetStatement*>(statement)!=0);
    REQUIRE(statement->token_literal()=="let");
    LetStatement* let_statement = (LetStatement*) statement;
    REQUIRE(let_statement->name->value == exp_ident);
    REQUIRE(let_statement->name->token_literal() == exp_ident);
};

void test_return_statement(Statement* statement){
    REQUIRE(dynamic_cast<ReturnStatement*>(statement)!=0);
    REQUIRE(statement->token_literal()=="return");
};

void test_expression_statement(Statement* statement){
    REQUIRE(dynamic_cast<ExpressionStatement*>(statement)!=0);
};

void test_identifier(Identifier* ident, string value){
    REQUIRE(ident->token_literal() == value);
    REQUIRE(ident->token.type == IDENT);
    REQUIRE(ident->value == value);
};

void check_parser_errors(Parser* p){
    REQUIRE(p->errors.size() == 0);
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
};

TEST_CASE("test return statement"){
    string input = ""
    "return 5;"
    "return 10;"
    "return 198374;";

    Program* program = get_program(input, 3);

    for(Statement* stmt : program->statements){
        test_return_statement(stmt);
    }
};

TEST_CASE("test 'to_string'"){
    Program* program = new Program();
    LetStatement* let_statement = new LetStatement();
    let_statement->token = Token{LET, "let"};
    Identifier* id1 = new Identifier();
    id1->token = Token{IDENT, "my_var"};
    id1->value = "my_var";
    Identifier* id2 = new Identifier();
    id2->token = Token{IDENT, "another_var"};
    id2->value = "another_var";
    let_statement->name = id1;
    let_statement->value = id2;
    program->statements = {
        let_statement
    };

    REQUIRE(program->to_string()=="let my_var = another_var; ");
};

TEST_CASE("test identifier expression"){
    string input = "foobar;";

    Program* program = get_program(input, 1);

    ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(program->statements[0]);
    REQUIRE(stmt != 0);

    Identifier* ident = dynamic_cast<Identifier*>(stmt->expression);
    REQUIRE(ident != 0);

    test_identifier(ident, "foobar");
};