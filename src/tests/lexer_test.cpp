#include <stdio.h>
#include <string>
#include "catch.hpp"
#include "../lexer.hpp"
#include <iostream>

using namespace std;


TEST_CASE("test next_token"){
    string input = "let five = 5;\n"
    "let ten = 10;\n"
    "let add = fn(x, y){\n"
    "  x + y;\n"
    "};\n"
    "let result = add(five, ten);\n"
    "!-/*5;\n"
    "5 < 10 > 5;\n"
    "if (5 == 10 != 2){\n"
    "  return true;\n"
    "} else {\n"
    "  return false;\n"
    "};\n"
    "\"foobar\""
    "\"foo bar\"";

    struct test{
        TokenType expected_type;
        string literal;
    };

    test let = {LET, "let"};
    test function = {FUNCTION, "fn"};
    test assign = {ASSIGN, "="};
    test lp = {LPAREN, "("};
    test rp = {RPAREN, ")"};
    test lb = {LBRACE, "{"};
    test rb = {RBRACE, "}"};
    test semi = {SEMICOLON, ";"};
    test comma = {COMMA, ","};
    test bang = {BANG, "!"};
    test plus = {PLUS, "+"};
    test minus = {MINUS, "-"};
    test slash = {SLASH, "/"};
    test asterix = {ASTERIX, "*"};
    test lt = {LT, "<"};
    test gt = {GT, ">"};
    test if_ = {IF, "if"};
    test else_ = {ELSE, "else"};
    test tru = {TRU, "true"};
    test fals = {FALS, "false"};
    test ret = {RETURN, "return"};
    test equal = {EQUAL, "=="};
    test not_equal = {NOT_EQUAL, "!="};

    vector<test> tests = {
        let, {IDENT, "five"}, assign, {INT, "5"}, semi,
        let, {IDENT, "ten"}, assign, {INT, "10"}, semi,
        let, {IDENT, "add"}, assign, function, lp, {IDENT, "x"}, comma, {IDENT, "y"}, rp, lb,
        {IDENT, "x"}, plus, {IDENT, "y"}, semi,
        rb, semi,
        let, {IDENT, "result"}, assign, {IDENT, "add"}, lp, {IDENT, "five"}, comma, {IDENT, "ten"}, rp, semi,
        bang, minus, slash, asterix, {INT, "5"}, semi,
        {INT, "5"}, lt, {INT, "10"}, gt, {INT, "5"}, semi,
        if_, lp, {INT, "5"}, equal, {INT, "10"}, not_equal, {INT, "2"}, rp, lb,
        ret, tru, semi,
        rb, else_, lb,
        ret, fals, semi,
        rb, semi,
        {STRING, "foobar"},
        {STRING, "foo bar"},
        {END, ""}
    };

    Lexer l(input);

    for(int i = 0; i<tests.size(); i++){
        Token tok = l.next_token();
        test t = tests[i];
        REQUIRE(tok.type == t.expected_type);
        REQUIRE(tok.literal == t.literal);
    }
}