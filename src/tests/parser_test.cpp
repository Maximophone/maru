#include "catch.hpp"
#include <string>
#include <vector>
#include "../parser.hpp"
#include "test_utils.cpp"

using namespace std;
using namespace std::literals::string_literals;

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

void test_identifier(Expression* exp, string value){
    Identifier* ident = dynamic_cast<Identifier*>(exp);
    REQUIRE(ident != 0);
    CHECK(ident->token_literal() == value);
    CHECK(ident->token.type == IDENT);
    CHECK(ident->value == value);
};

void test_integer_literal(Expression* exp, int value){
    IntegerLiteral* literal = dynamic_cast<IntegerLiteral*>(exp);
    REQUIRE(literal != 0);
    CHECK(literal->value == value);
    CHECK(literal->token_literal() == to_string(value));
};

void test_boolean_literal(Expression* exp, bool value){
    INFO("Testing boolean literal");
    BooleanLiteral* literal = dynamic_cast<BooleanLiteral*>(exp);
    REQUIRE(literal != 0);
    CHECK(literal->value == value);
    CHECK(literal->token_literal() == (value?"true":"false"));
};

void test_string_literal(Expression* exp, string value){
    INFO("Testing string literal");
    StringLiteral* literal = dynamic_cast<StringLiteral*>(exp);
    REQUIRE(literal != 0);
    CHECK(literal->value == value);
    CHECK(literal->token_literal() == value);
}

void check_parser_errors(Parser* p){
    INFO("Parsing errors happened");
    string error_messages = "";
    for(string error : p->errors){
        error_messages += error + "\n";
    }
    INFO(error_messages);
    REQUIRE(p->errors.size() == 0);
};

void test_var_literal(Expression* exp, Var literal){
        switch(literal.type){
            case 'i':
                test_integer_literal(exp, literal.i);
                break;
            case 'b':
                test_boolean_literal(exp, literal.b);
                break;
            case 's':
                test_identifier(exp, literal.s);
                break;
        }
}

TEST_CASE("test let statements"){
    struct test {
        string input;
        string expected_ident;
        Var value;
    };
    vector<test> tests = {
        {"let x = 5;", "x", Var(5)},
        {"let y = true;", "y", Var(true)},
        {"let foobar = y", "foobar", Var("y"s)},
    };

    for(test t: tests){
        INFO("INPUT: " + t.input)
        INFO(t.value.type);
        INFO(t.value.s);
        Program* p = get_program(t.input, 1);
        LetStatement* stmt = dynamic_cast<LetStatement*>(p->statements[0]);
        REQUIRE(stmt != 0);
        test_let_statement(stmt, t.expected_ident);
        test_var_literal(stmt->value, t.value);
    }
};

TEST_CASE("test return statement"){
    struct test {
        string input;
        Var value;
    };
    vector<test> tests = {
        {"return 5;", Var(5)},
        {"return a;", Var("a"s)},
        {"return true;", Var(true)},
    };

    for(test t : tests){
        Program* p = get_program(t.input, 1);
        ReturnStatement* stmt = dynamic_cast<ReturnStatement*>(p->statements[0]);
        REQUIRE(stmt != 0);
        test_var_literal(stmt->return_value, t.value);
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

    test_identifier(stmt->expression, "foobar");
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
        {
            "1 * (2 + 3 ) - 4",
            "((1*(2+3))-4)",
        },
        {
            "(5+5)*2",
            "((5+5)*2)",
        },
        {
            "a + add(b*c) + d",
            "((a+add((b*c)))+d)",
        },
        {
            "add(a, b, 1, 2*3, 4+5, add(6, 7*8))",
            "add(a,b,1,(2*3),(4+5),add(6,(7*8)))",
        }
    };

    for(test t : tests){
        Program* program = get_program(t.input, -1);
        string actual = program->to_string();
        INFO("parsed program string does not match expected");
        CHECK(actual == t.expected);
    };
};

template <typename T>
void test_variable_literal(Expression* exp, T literal){};

template<>
void test_variable_literal<string>(Expression* exp, string literal){
    test_identifier(exp, literal);
};

template<>
void test_variable_literal<int>(Expression* exp, int literal){
    test_integer_literal(exp, literal);
};

template<>
void test_variable_literal<bool>(Expression* exp, bool literal){
    test_boolean_literal(exp, literal);
};

template<typename T_left, typename T_right>
void test_infix_expression(Expression* exp, T_left left, string op, T_right right){
    InfixExpression* inf_exp = dynamic_cast<InfixExpression*>(exp);
    REQUIRE(inf_exp != 0);
    test_variable_literal<T_left>(inf_exp->left_value, left);
    CHECK(inf_exp->op == op);
    test_variable_literal<T_right>(inf_exp->right_value, right);
};

TEST_CASE("test if expression"){
    string input = "if(x<y){x}";

    Program* program = get_program(input, 1);

    ExpressionStatement* stmt = get_first_expr_stmt(program);

    IfExpression* exp = dynamic_cast<IfExpression*>(stmt->expression);
    REQUIRE(exp != 0);

    test_infix_expression<string, string>(exp->condition, "x", "<", "y");

    BlockStatement* cons = dynamic_cast<BlockStatement*>(exp->consequence);
    REQUIRE(cons != 0);
    REQUIRE(cons->statements.size()==1);

    ExpressionStatement* exp_stmt = dynamic_cast<ExpressionStatement*>(cons->statements[0]);
    REQUIRE(exp_stmt != 0);

    test_identifier(exp_stmt->expression, "x");

    REQUIRE(exp->alternative == 0);
};

TEST_CASE("test if else expression"){
    string input = "if(x<y){x}else{y}";

    Program* program = get_program(input, 1);

    ExpressionStatement* stmt = get_first_expr_stmt(program);

    IfExpression* exp = dynamic_cast<IfExpression*>(stmt->expression);
    REQUIRE(exp != 0);

    test_infix_expression<string, string>(exp->condition, "x", "<", "y");

    BlockStatement* cons = dynamic_cast<BlockStatement*>(exp->consequence);
    REQUIRE(cons != 0);
    REQUIRE(cons->statements.size()==1);

    ExpressionStatement* exp_stmt = dynamic_cast<ExpressionStatement*>(cons->statements[0]);
    REQUIRE(exp_stmt != 0);

    test_identifier(exp_stmt->expression, "x");

    BlockStatement* alt = dynamic_cast<BlockStatement*>(exp->alternative);
    REQUIRE(alt != 0);
    REQUIRE(alt->statements.size()==1);

    ExpressionStatement* exp_stmt_alt = dynamic_cast<ExpressionStatement*>(alt->statements[0]);
    REQUIRE(exp_stmt_alt != 0);

    test_identifier(exp_stmt_alt->expression, "y");
};

TEST_CASE("test function literal parsing"){
    string input = "fn(x, y){x + y;}";

    Program* program = get_program(input, 1);

    ExpressionStatement* stmt = get_first_expr_stmt(program);

    FunctionLiteral* fn = dynamic_cast<FunctionLiteral*>(stmt->expression);
    REQUIRE(fn!=0);

    INFO("wrong number of parameters");
    CHECK(fn->parameters.size()==2);

    INFO("wrong parameters");
    test_identifier(fn->parameters[0], "x");
    test_identifier(fn->parameters[1], "y");

    INFO("wrong body");
    BlockStatement* body = dynamic_cast<BlockStatement*>(fn->body);
    REQUIRE(body->statements.size() == 1);

    ExpressionStatement* exp = dynamic_cast<ExpressionStatement*>(body->statements[0]);
    REQUIRE(exp!=0);

    test_infix_expression<string, string>(exp->expression, "x", "+", "y");

};

TEST_CASE("test function parameter parsing"){
    struct test {
        string input;
        vector<string> expected_params;
    };
    vector<test> tests = {
        {"fn(){};", {}},
        {"fn(x){};", {"x"}},
        {"fn(x, y, z){};", {"x", "y", "z"}},
    };

    for(test t : tests){
        Program* p = get_program(t.input, 1);
        ExpressionStatement* stmt = get_first_expr_stmt(p);
        FunctionLiteral* fn = dynamic_cast<FunctionLiteral*>(stmt->expression);
        REQUIRE(fn!=0);
        CHECK(fn->parameters.size() == t.expected_params.size());
        for(int i = 0; i<fn->parameters.size(); i++){
            Identifier* ident = fn->parameters[i];
            test_identifier(ident, t.expected_params[i]);
        }
    };
};

TEST_CASE("test call expression parsing"){
    string input = "add(1, 2*3, 4+5)";
    Program* p = get_program(input, 1);
    ExpressionStatement* stmt = get_first_expr_stmt(p);

    CallExpression* exp = dynamic_cast<CallExpression*>(stmt->expression);
    REQUIRE(exp != 0);

    test_identifier(exp->function, "add");

    REQUIRE(exp->arguments.size() == 3);
    test_integer_literal(exp->arguments[0], 1);
    test_infix_expression<int, int>(exp->arguments[1], 2, "*", 3);
    test_infix_expression<int, int>(exp->arguments[2], 4, "+", 5);
};

TEST_CASE("test string literal expression"){
    string input = "\"hello world\"";

    Program* p = get_program(input, 1);
    ExpressionStatement* stmt = get_first_expr_stmt(p);

    test_string_literal(stmt->expression, "hello world");
}