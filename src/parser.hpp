#ifndef PARSER_H
#define PARSER_H

#include "ast.hpp"

class Parser;

typedef Expression* (Parser::*prefix_parse_func)();
typedef Expression* (Parser::*infix_parse_func)(Expression*);

const int LOWEST = 0;
const int ASSIGNMENT = 1;
const int EQUALS = 2;
const int LESSGREATER = 3;
const int SUM = 4;
const int PRODUCT = 5;
const int PREFIX = 6;
const int CALL = 8;
const int INDEX = 9;
const int ATTR = 10;

extern map<TokenType, int> precedences;

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
        Statement* parse_continue_break_statement(TokenType);
        ExpressionStatement* parse_expression_statement();
        Expression* parse_expression(int);
        Expression* parse_assign_expression(Expression*);
        Expression* parse_identifier();
        Expression* parse_integer_literal();
        Expression* parse_float_literal();
        Expression* parse_boolean_literal();
        Expression* parse_string_literal();
        Expression* parse_array_literal();
        Expression* parse_hash_literal();
        Expression* parse_function_literal();
        Expression* parse_test_expression();
        vector<Identifier*> parse_function_parameters(bool&);
        Expression* parse_call_expression(Expression*);
        Expression* parse_class_literal();
        Expression* parse_index_expression(Expression*);
        Expression* parse_access_expression(Expression*);
        Expression* parse_if_expression();
        Expression* parse_for_expression();
        Expression* parse_while_expression();
        BlockStatement* parse_block_statement();
        Expression* parse_prefix_expression();
        Expression* parse_infix_expression(Expression*);
        Expression* parse_grouped_expression();
        vector<Expression*> parse_expression_list(TokenType, bool&);
        IncludeStatement* parse_include_statement();
        void no_prefix_parse_func_error(Token);
        bool cur_token_is(TokenType);
        bool peek_token_is(TokenType);
        bool expect_peek(TokenType);
        void peek_error(TokenType);
        int cur_precedence();
        int peek_precedence();
    public:
        Parser(Lexer*);
        Program* parse_program();
        vector<string> errors = {};
};

#endif