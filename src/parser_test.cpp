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
    if(wanted_size >= 0){
        REQUIRE(program->statements.size() == wanted_size);
    }

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

void test_integer_literal(Expression* exp, int value){
    IntegerLiteral* literal = dynamic_cast<IntegerLiteral*>(exp);
    REQUIRE(literal != 0);
    REQUIRE(literal->value == value);
    REQUIRE(literal->token_literal() == to_string(value));
};

void test_boolean_literal(Expression* exp, bool value){
    INFO("Testing boolean literal");
    BooleanLiteral* literal = dynamic_cast<BooleanLiteral*>(exp);
    REQUIRE(literal != 0);
    CHECK(literal->value == value);
    CHECK(literal->token_literal() == (value?"true":"false"));
};

void check_parser_errors(Parser* p){
    INFO("Parsing errors happened");
    string error_messages = "";
    for(string error : p->errors){
        error_messages += error + "\n";
    }
    INFO(error_messages);
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

ExpressionStatement* get_expr_stmt(Program* program, int position){
    ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(program->statements[position]);
    REQUIRE(stmt != 0);
    return stmt;
};

ExpressionStatement* get_first_expr_stmt(Program* program){
    return get_expr_stmt(program, 0);
};

TEST_CASE("test integer literal"){
    string input = "5;";

    Program* program = get_program(input, 1);

    ExpressionStatement* stmt = get_first_expr_stmt(program);

    test_integer_literal(stmt->expression, 5);
};

TEST_CASE("test boolean literal"){
    string input = "true;false;";

    Program* program = get_program(input, 2);

    ExpressionStatement* stmt = get_expr_stmt(program, 0);
    test_boolean_literal(stmt->expression, true);

    stmt = get_expr_stmt(program, 1);
    test_boolean_literal(stmt->expression, false);
};

TEST_CASE("test parsing prefix expressions"){
    struct test{
        string input;
        string op;
        int int_value;
    };
    vector<test> tests = {
        {"!5;", "!", 5},
        {"-15", "-", 15},
    };

    for(test t : tests){
        Program* program = get_program(t.input, 1);
        ExpressionStatement* stmt = get_first_expr_stmt(program);
        PrefixExpression* exp = dynamic_cast<PrefixExpression*>(stmt->expression);
        REQUIRE(exp->op == t.op);
        test_integer_literal(exp->right, t.int_value);
    };
};

TEST_CASE("test parsing infix expressions"){
    struct test{
        string input;
        int left_value;
        string op;
        int right_value;
    };
    vector<test> tests = {
        {"5+5", 5, "+", 5},
        {"5-5", 5, "-", 5},
        {"5 * 5", 5, "*", 5},
        {"5/ 5", 5, "/", 5},
        {"5<2", 5, "<", 2},
        {" 5> 5", 5, ">", 5},
        {"5 == 5", 5, "==", 5},
        {"5 != 5", 5, "!=", 5},
    };

    for(test t : tests){
        Program* program = get_program(t.input, 1);
        ExpressionStatement* stmt = get_first_expr_stmt(program);
        InfixExpression* exp = dynamic_cast<InfixExpression*>(stmt->expression);
        test_integer_literal(exp->left_value, t.left_value);
        REQUIRE(exp->op == t.op);
        test_integer_literal(exp->right_value, t.right_value);
    };
};

TEST_CASE("test operator precedence parsing"){
    struct test{
        string input;
        string expected;
    };
    vector<test> tests = {
        {
            "-a * b",
            "((-a)*b)",
        },
        {
            "!-a",
            "(!(-a))",
        },
        {
            "a + b+c;",
            "((a+b)+c)",
        },
        {
            "a+b-c",
            "((a+b)-c)",
        },
        {
            "a+b*c",
            "(a+(b*c))",
        },
        {
            "a + b/c",
            "(a+(b/c))",
        },
        {
            "5<4 != 3 > 4",
            "((5<4)!=(3>4))",
        },
        {
            "5<4 == 3>4",
            "((5<4)==(3>4))",
        },
        {
            "3+4; -5*5",
            "(3+4)((-5)*5)",
        },
        {
            "true + false == true",
            "((true+false)==true)",
        },
    };

    for(test t : tests){
        Program* program = get_program(t.input, -1);
        string actual = program->to_string();
        INFO("parsed program string does not match expected");
        CHECK(actual == t.expected);
    };
};