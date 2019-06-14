#include "token.hpp"

const TokenType test = "blah";

TokenType lookup_ident(string ident){
    if(keywords.count(ident)){
        return keywords[ident];
    }
    return IDENT;
};

map<string, TokenType> keywords = {
    {"fn", FUNCTION},
    {"let", LET},
    {"return", RETURN},
    {"if", IF},
    {"else", ELSE},
    {"true", TRU},
    {"false", FALS},
};