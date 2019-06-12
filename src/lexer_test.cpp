#include <stdio.h>
#include <string>
#include "catch.hpp"
#include "token.hpp"
#include "lexer.hpp"
#define CATCH_CONFIG_MAIN

using namespace std;

int tests_run = 0;
int foo = 5;

TEST_CASE("test next_token"){
    string input = "=*(){},;";
    struct test{
        TokenType expected_type;
        string literal;
    };
    struct test tests[] = {
        {ASSIGN, "="},
        {PLUS, "="}
    };

    l = new Lexer(input);

    for(int i = 0; i<=std::size_t(tests); i++){ 
        Token tok = l.next_token();
        test t = tests[i];
        REQUIRE(tok.type == t.expected_type);
        REQUIRE(tok.literal == t.literal);
    }
}