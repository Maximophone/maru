#include <string>
#include "catch.hpp"

using namespace std;

class Var{
    public:
        int i;
        string s;
        bool b;
        char type;
    Var(){type='n';};
    Var(int i_){type='i'; i=i_;};
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