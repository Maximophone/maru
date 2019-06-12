#include <string>

using namespace std;

string x = "test";

typedef string TokenType;

const TokenType ILLEGAL = "ILLEGAL";
const TokenType END = "EOF";

const TokenType IDENT = "IDENT";
const TokenType INT = "INT";

const TokenType ASSIGN = "ASSIGN";
const TokenType PLUS = "PLUS";

const TokenType COMMA = "COMMA";
const TokenType SEMICOLON = "SEMICOLON";
const TokenType LPAREN = "LPAREN";
const TokenType RPAREN = "RPAREN";
const TokenType LBRACE = "LBRACE";
const TokenType RBRACE = "RBRACE";

const TokenType FUNCTION = "FUNCTION";
const TokenType LET = "LET";

struct Token {
    TokenType type;
    string literal;
};