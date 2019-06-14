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

extern const TokenType ILLEGAL;
extern const TokenType END;

extern const TokenType IDENT;
extern const TokenType INT;

extern const TokenType ASSIGN;
extern const TokenType PLUS;
extern const TokenType MINUS;
extern const TokenType SLASH;
extern const TokenType BANG;
extern const TokenType ASTERIX;
extern const TokenType LT;
extern const TokenType GT;

extern const TokenType EQUAL;
extern const TokenType NOT_EQUAL;

extern const TokenType COMMA;
extern const TokenType SEMICOLON;
extern const TokenType LPAREN;
extern const TokenType RPAREN;
extern const TokenType LBRACE;
extern const TokenType RBRACE;

extern const TokenType FUNCTION;
extern const TokenType LET;
extern const TokenType RETURN;
extern const TokenType IF;
extern const TokenType ELSE;
extern const TokenType TRU;
extern const TokenType FALS;