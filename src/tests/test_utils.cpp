#include "test_utils.hpp"

void test_identifier(Expression* exp, string value){
    Identifier* ident = dynamic_cast<Identifier*>(exp);
    REQUIRE(ident != 0);
    CHECK(ident->token_literal() == value);
    CHECK(ident->token.type == IDENT);
    CHECK(ident->value == value);
};