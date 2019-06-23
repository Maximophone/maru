#ifndef AST_H
#define AST_H

#include <string>
#include "lexer.hpp"
#include <vector>

using namespace std;

class BlockStatement;

class Node{
    public:
        virtual string token_literal() = 0;
        virtual string to_string() = 0;
};

class Expression: public Node{
    public:
        Token token;
        string token_literal();
};

class Statement: public Node{
    public:
        Token token;
        string token_literal();
};

class Program: public Node {
    public:
        vector<Statement*> statements;
        string token_literal();
        string to_string();
};

class Identifier: public Expression{
    public:
        string value;
        string to_string();
};

class IntegerLiteral: public Expression{
    public:
        int value;
        string to_string();
};

class BooleanLiteral: public Expression{
    public:
        bool value;
        string to_string();
};

class StringLiteral: public Expression{
    public:
        string value;
        string to_string();
};

class ArrayLiteral: public Expression{
    public:
        vector<Expression*> elements;
        string to_string();
};

class HashLiteral: public Expression{
    public:
        map<Expression*, Expression*> pairs;
        string to_string();
};

class FunctionLiteral: public Expression{
    public:
        vector<Identifier*> parameters;
        BlockStatement* body;
        string to_string();      
};

class AssignExpression: public Expression{
    public:
        Identifier* name;
        Expression* value;
        string to_string();
};

class CallExpression: public Expression{
    public:
        Expression* function;
        vector<Expression*> arguments;
        string to_string();
};

class IndexExpression: public Expression{
    public:
        Expression* left;
        Expression* index;
        string to_string();
};

class PrefixExpression: public Expression{
    public:
        string op;
        Expression* right;
        string to_string();
};

class InfixExpression: public Expression{
    public:
        Expression* left_value;
        string op;
        Expression* right_value;
        string to_string();
};

class BlockStatement: public Statement{
    public:
        vector<Statement*> statements;
        string to_string();
};

class IfExpression: public Expression{
    public:
        Expression* condition;
        BlockStatement* consequence;
        BlockStatement* alternative;
        string to_string();
};

class ForExpression: public Expression{
    public:
        Identifier* iterator;
        Expression* iterated;
        BlockStatement* body;
        string to_string();
};

class LetStatement: public Statement{
    public:
        Identifier* name;
        Expression* value;
        string to_string();
};

class ReturnStatement: public Statement{
    public:
        Expression* return_value;
        string to_string();
};

class ExpressionStatement: public Statement{
    public:
        Expression* expression;
        string to_string();
};

#endif