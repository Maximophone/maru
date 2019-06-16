#ifndef PARSER_H
#define PARSER_H

#include "ast.hpp"

class Parser;

typedef Expression* (Parser::*prefix_parse_func)();
typedef Expression* (Parser::*infix_parse_func)(Expression*);

const int LOWEST = 1;
const int EQUALS = 2;
const int LESSGREATER = 3;
const int SUM = 4;
const int PRODUCT = 5;
const int PREFIX = 6;
const int CALL = 7;

class Parser{
    private:
        Lexer *lexer;
        Token cur_token;
        Token peek_token;
        map<TokenType, prefix_parse_func> prefix_parse_funcs;
        map<TokenType, infix_parse_func> infix_parse_funcs;
        void next_token();
        Statement* parse_statement();
        LetStatement* parse_let_statement();
        ReturnStatement* parse_return_statement();
        ExpressionStatement* parse_expression_statement();
        Expression* parse_expression(int);
        Expression* parse_identifier();
        Expression* parse_integer_literal();
        Expression* parse_prefix_expression();
        void no_prefix_parse_func_error(TokenType);
        bool cur_token_is(TokenType);
        bool peek_token_is(TokenType);
        bool expect_peek(TokenType);
        void peek_error(TokenType);
    public:
        Parser(Lexer*);
        Program* parse_program();
        vector<string> errors = {};
};

#endif