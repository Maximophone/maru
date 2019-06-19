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

    return eval(dynamic_cast<Node*>(program));
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