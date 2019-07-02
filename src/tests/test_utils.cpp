#include "test_utils.hpp"
#include "../object.hpp"
#include "../evaluator.hpp"

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


void test_not_error(Object* obj){
    if(Error* err = dynamic_cast<Error*>(obj)){
        INFO("Evaluation returned an error: " + err->message);
        REQUIRE(false);
    }
}

void test_integer_object(Object* obj, int expected){
    INFO("Testing integer object");

    test_not_error(obj);
    Integer* int_obj = dynamic_cast<Integer*>(obj);
    REQUIRE(int_obj != 0);
    CHECK(int_obj->value == expected);
};

void test_boolean_object(Object* obj, bool expected){
    INFO("Testing boolean object");

    test_not_error(obj);
    Boolean* bool_obj = dynamic_cast<Boolean*>(obj);
    REQUIRE(bool_obj != 0);
    CHECK(bool_obj->value == expected);
};

void test_string_object(Object* obj, string expected){
    INFO("Testing string object");

    test_not_error(obj);
    String* string_obj = dynamic_cast<String*>(obj);
    REQUIRE(string_obj != 0);
    CHECK(string_obj->value == expected);
};

void test_null_object(Object* obj){
    INFO("Testing NULL object");

    test_not_error(obj);
    Null* null_obj = dynamic_cast<Null*>(obj);
    REQUIRE(null_obj != 0);
    CHECK(null_obj == NULL_);
};

void test_error_object(Object* obj, string expected_message){
    INFO("Testing ERROR object");

    Error* error_obj = dynamic_cast<Error*>(obj);
    REQUIRE(error_obj != 0);
    CHECK(error_obj->message == expected_message);
};

void test_var_object(Object* obj, Var expected){
    switch(expected.type){
        case 'n':
            return test_null_object(obj);
            break;
        case 'i':
            return test_integer_object(obj, expected.i);
            break;
        case 'b':
            return test_boolean_object(obj, expected.b);
            break;
        case 's':
            return test_error_object(obj, expected.s);
            break;
    }
};