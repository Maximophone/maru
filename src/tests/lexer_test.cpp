#include <stdio.h>
#include <string>
#include "catch.hpp"
#include "../lexer.hpp"
#include <iostream>

using namespace std;


TEST_CASE("test next_token"){
    string input = "let five = 5;\n"
    "# this is a comment, it should all be ignored;\n"
    "let ten = 10; # another comment...\n"
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
    "\"foo bar\""
    "[1, 2]"
    "{1: 3}"
    "for(x in range(5)){break}"
    "while(x>1){continue}"
    "class Test{"
    " a;"
    " b=1;"
    "}"
    "a.x;"
    "include x;"
    "include a as b;"
    "1.5;"
    "1.8.3;"
    "test(x){};"
    ;

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
    test colon = {COLON, ":"};
    test bang = {BANG, "!"};
    test plus = {PLUS, "+"};
    test minus = {MINUS, "-"};
    test slash = {SLASH, "/"};
    test asterix = {ASTERIX, "*"};
    test lt = {LT, "<"};
    test gt = {GT, ">"};
    test lbra = {LBRACKET, "["};
    test rbra = {RBRACKET, "]"};
    test if_ = {IF, "if"};
    test else_ = {ELSE, "else"};
    test tru = {TRU, "true"};
    test fals = {FALS, "false"};
    test ret = {RETURN, "return"};
    test equal = {EQUAL, "=="};
    test not_equal = {NOT_EQUAL, "!="};
    test for_ = {FOR, "for"};
    test in_ = {IN, "in"};
    test while_ = {WHILE, "while"};
    test x_ = {IDENT, "x"};
    test b_ = {IDENT ,"b"};
    test a_ = {IDENT, "a"};
    test one_ = {INT, "1"};
    test class_ = {CLASS, "class"};
    test dot_ = {DOT, "."};
    test break_ = {BREAK, "break"};
    test continue_ = {CONTINUE, "continue"};
    test include_ = {INCLUDE, "include"};
    test as_ = {AS, "as"};
    test tst = {TEST, "test"};

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
        lbra, {INT, "1"}, comma, {INT, "2"}, rbra,
        lb, {INT, "1"}, colon, {INT, "3"}, rb,
        for_, lp, {IDENT, "x"}, in_, {IDENT, "range"}, lp, {INT, "5"}, rp, rp, lb, break_, rb,
        while_, lp, x_, gt, one_, rp, lb, continue_, rb,
        class_, {IDENT, "Test"}, lb,
        a_, semi,
        b_, assign, one_, semi,
        rb,
        a_, dot_, x_, semi,
        include_, x_, semi,
        include_, a_, as_, b_, semi,
        {FLOAT, "1.5"}, semi,
        {FLOAT, "1.8.3"}, semi,
        tst, lp, x_, rp, lb, rb, semi,
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