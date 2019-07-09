#include <string>
#include "catch.hpp"
#include "../parser.hpp"
#include "../object.hpp"

using namespace std;

class Var{
    public:
        int i;
        double f;
        string s;
        bool b;
        char type;
    Var(){type='n';};
    Var(int i_){type='i'; i=i_;};
    Var(double f_){type='f'; f=f_;};
    Var(string s_){type='s'; s=s_;};
    Var(bool b_){type='b'; b=b_;};
};

template<typename T_new, typename T_old>
T_new req_cast(T_old obj){
    INFO("Can't cast object");
    T_new casted = dynamic_cast<T_new>(obj);
    REQUIRE(casted != 0);
    return casted;
};

Object* test_eval(string);
void test_identifier(Expression*, string);
void check_parser_errors(Parser*);
void test_not_error(Object*);
void test_integer_object(Object*, int);
void test_float_object(Object*, double);
void test_boolean_object(Object*, bool);
void test_string_object(Object*, string);
void test_null_object(Object*);
void test_error_object(Object*, string);
void test_var_object(Object*, Var);