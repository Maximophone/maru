#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <map>

using namespace std;

typedef string TokenType;

class Token{
    public:
        TokenType type;
        string literal;
};

TokenType lookup_ident(string);

extern map<string, TokenType> keywords;
extern const TokenType test;

const TokenType ILLEGAL = "ILLEGAL";
const TokenType END = "EOF";

const TokenType IDENT = "IDENT";
const TokenType INT = "INT";
const TokenType STRING = "STRING";

const TokenType ASSIGN = "ASSIGN";
const TokenType PLUS = "PLUS";
const TokenType MINUS = "MINUS";
const TokenType SLASH = "SLASH";
const TokenType BANG = "BANG";
const TokenType ASTERIX = "ASTERIX";
const TokenType LT = "LT";
const TokenType GT = "GT";

const TokenType EQUAL = "EQUAL";
const TokenType NOT_EQUAL = "NOT_EQUAL";

const TokenType COMMA = "COMMA";
const TokenType SEMICOLON = "SEMICOLON";
const TokenType LPAREN = "LPAREN";
const TokenType RPAREN = "RPAREN";
const TokenType LBRACE = "LBRACE";
const TokenType RBRACE = "RBRACE";

const TokenType FUNCTION = "FUNCTION";
const TokenType LET = "LET";
const TokenType RETURN = "RETURN";
const TokenType IF = "IF";
const TokenType ELSE = "ELSE";
const TokenType TRU = "TRUE";
const TokenType FALS = "FALSE";

#endif