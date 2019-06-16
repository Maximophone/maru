#include <string>
#include "lexer.hpp"
#include <vector>

using namespace std;

class Node{
    public:
        virtual string token_literal(){};
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

