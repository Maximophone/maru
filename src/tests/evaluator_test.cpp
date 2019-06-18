#include "catch.hpp"
#include "../evaluator.hpp"
#include "../parser.hpp"
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
}

TEST_CASE("test eval integer expression"){
    struct test {
        string input;
        int expected;
    };
    vector<test> tests = {
        {"5", 5},
        {"10", 10},
    };

    for(test t : tests){
        Object* evaluated = test_eval(t.input);
        test_integer_object(evaluated, t.expected);
    }
};
