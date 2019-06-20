#include "catch.hpp"
#include "../evaluator.hpp"
#include "../parser.hpp"
#include "test_utils.cpp"
#include <string>

using namespace std;

Object* test_eval(string input){
    Lexer* l = new Lexer(input);
    Parser* p = new Parser(l);
    Program* program = p->parse_program();

    Environment* env = new Environment();
    return eval(dynamic_cast<Node*>(program), env);
};

void test_integer_object(Object* obj, int expected){
    INFO("Testing integer object");

    Integer* int_obj = dynamic_cast<Integer*>(obj);
    REQUIRE(int_obj != 0);
    CHECK(int_obj->value == expected);
};

void test_boolean_object(Object* obj, bool expected){
    INFO("Testing boolean object");

    Boolean* bool_obj = dynamic_cast<Boolean*>(obj);
    REQUIRE(bool_obj != 0);
    CHECK(bool_obj->value == expected);
};

void test_null_object(Object* obj){
    INFO("Testing NULL object");

    Null* null_obj = dynamic_cast<Null*>(obj);
    REQUIRE(null_obj != 0);
    CHECK(null_obj == NULL_);
};

void test_error_object(Object* obj, string expected_message){
    INFO("Testing ERROR object");

    Error* error_obj = dynamic_cast<Error*>(obj);
    REQUIRE(error_obj != 0);
    CHECK(error_obj->message == expected_message);
};

void test_var_object(Object* obj, Var expected){
    switch(expected.type){
        case 'n':
            return test_null_object(obj);
            break;
        case 'i':
            return test_integer_object(obj, expected.i);
            break;
        case 'b':
            return test_boolean_object(obj, expected.b);
            break;
    }
};

TEST_CASE("test eval integer expression"){
    struct test {
        string input;
        int expected;
    };
    vector<test> tests = {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
        {"5 + 5 + 5 + 5 - 10", 10},
        {"2 * 2 * 2 * 2 * 2", 32},
        {"-50 + 100 + -50", 0},
        {"5 * 2 + 10", 20},
        {"5 + 2 * 10", 25},
        {"20 + 2 * -10", 0},
        {"50 / 2 * 2 + 10", 60},
        {"2 * (5 + 10)", 30},
        {"3 * 3 * 3 + 10", 37},
        {"3 * (3 * 3) + 10", 37},
        {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
    };

    for(test t : tests){
        Object* evaluated = test_eval(t.input);
        test_integer_object(evaluated, t.expected);
    }
};

TEST_CASE("test eval boolean expression"){
    struct test {
        string input;
        bool expected;
    };
    vector<test> tests = {
        {"true", true},
        {"false", false},
        {"1 <2", true},
        {"1 >2", false},
        {"1 <1", false},
        {"1>1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 == 2", false},
        {"1 != 2", true},
        {"true == true", true},
        {"false == false", true},
        {"true == false", false},
        {"true != false", true},
        {"false != true", true},
        {"(1 <2) == true", true},
        {"(1 <2) == false", false},
        {"(1 >2) == true", false},
        {"(1 >2) == false", true},
    };

    for(test t : tests){
        Object* evaluated = test_eval(t.input);
        test_boolean_object(evaluated, t.expected);
    }
};

TEST_CASE("test bang operator"){
    struct test {
        string input;
        bool expected;
    };
    vector<test> tests = {
        {"!true", false},
        {"!false", true},
        {"!5", false},
        {"!!true", true},
        {"!!false", false},
        {"!!5", true},
    };

    for(test t : tests){
        Object* evaluated = test_eval(t.input);
        test_boolean_object(evaluated, t.expected);
    }
};

TEST_CASE("test if else expressions"){
    struct test{
        string input;
        Var expected;
    };
    vector<test> tests = {
        {"if(true) {10}", Var(10)},
        {"if(false){8}", Var()},
        {"if (1) {10}", Var(10)},
        {"if (1 < 2) { 10 }", Var(10)},
        {"if (1 > 2) { 10 }", Var()},
        {"if (1 > 2) { 10 } else { 20 }", Var(20)},
        {"if (1 < 2) { 10 } else { 20 }", Var(10)},
    };

    for (test t : tests){
        INFO("Expression: " + t.input);
        Object* evaluated = test_eval(t.input);
        test_var_object(evaluated, t.expected);
    }
};

TEST_CASE("test return statements"){
    struct test{
        string input;
        int expected;
    };
    vector<test> tests = {
        {"return 10;", 10},
        {"return 10; 9;", 10},
        {"return 2 * 5; 9;", 10},
        {"9; return 2 * 5; 9;", 10},
        {"if (10 > 1) {"
            "if (10 > 1) {"
                "return 10;"
            "}"
            "return 1;"
        "}", 10},
    };

    for(test t : tests){
        INFO("Expression: " + t.input);
        Object* evaluated = test_eval(t.input);
        test_integer_object(evaluated, t.expected);
    }
};

TEST_CASE("test error handling"){
    struct test {
        string input;
        string expected_message;
    };
    vector<test> tests = {
        {
            "5 + true;",
            "type mismatch: INTEGER+BOOLEAN",
        },
        {
            "5 + true; 5;",
            "type mismatch: INTEGER+BOOLEAN",
        },
        {
            "-true",
            "unknown operator: -BOOLEAN",
        },
        {
            "true + false;",
            "unknown operator: BOOLEAN+BOOLEAN",
        },
        {
            "5; true + false; 5",
            "unknown operator: BOOLEAN+BOOLEAN",
        },
        {
            "if (10 > 1) { true + false; }",
            "unknown operator: BOOLEAN+BOOLEAN",
        },
        {
"if (10 > 1) {"
  "if (10 > 1) {"
    "return true + false;"
  "}"
  "return 1;"
"}",
            "unknown operator: BOOLEAN+BOOLEAN",
        },
        {"foobar", "identifier not found: foobar"},
    };

    for(test t : tests){
        INFO("Input: " + t.input);
        Object* evaluated = test_eval(t.input);
        test_error_object(evaluated, t.expected_message);
    }
};

TEST_CASE("test evaluating let statements"){
    struct test {
        string input;
        int expected;
    };
    vector<test> tests = {
        {"let a = 5; a;", 5},
        {"let a = 5 * 5; a;", 25},
        {"let a = 5; let b = a; b;", 5},
        {"let a = 5; let b = a; let c = a + b + 5; c;", 15},
    };
    for(test t : tests){
        test_integer_object(test_eval(t.input), t.expected);
    };
};