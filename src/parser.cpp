#include "parser.hpp"
#include "lexer.hpp"

Parser::Parser(Lexer *l){
    lexer = l;
    next_token();
    next_token();
};

void Parser::next_token(){
    cur_token = peek_token;
    peek_token = lexer->next_token();
};

Program* Parser::parse_program(){
    return 0;
};