#include "test_utils.hpp"

void check_parser_errors(Parser* p){
    INFO("Parsing errors happened");
    string error_messages = "";
    for(string error : p->errors){
        error_messages += error + "\n";
    }
    INFO(error_messages);
    REQUIRE(p->errors.size() == 0);
};

void test_identifier(Expression* exp, string value){
    Identifier* ident = dynamic_cast<Identifier*>(exp);
    REQUIRE(ident != 0);
    CHECK(ident->token_literal() == value);
    CHECK(ident->token.type == IDENT);
    CHECK(ident->value == value);
};