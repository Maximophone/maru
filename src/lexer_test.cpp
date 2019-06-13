#define CATCH_CONFIG_MAIN
#include <stdio.h>
#include <string>
#include "catch.hpp"
#include "lexer.hpp"
#include <iostream>

using namespace std;


TEST_CASE("test next_token"){
    string input = "let five = 5;";
    struct test{
        TokenType expected_type;
        string literal;
    };
    vector<test> tests = {
        {LET, "let"},
        {IDENT, "five"},
        {ASSIGN, "="},
        {INT, "5"},
        {SEMICOLON, ";"},
    };

    Lexer l(input);

    for(int i = 0; i<tests.size(); i++){
        Token tok = l.next_token();
        test t = tests[i];
        REQUIRE(tok.type == t.expected_type);
        REQUIRE(tok.literal == t.literal);
    }
}