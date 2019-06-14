#include <string>
#include "lexer.hpp"
#include <vector>

using namespace std;

class Node{
    public:
        virtual string token_literal();
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
};

class Identifier: public Expression{
    public:
        string value;
};

class LetStatement: public Statement{
    public:
        Identifier* name;
        Expression* value;
};