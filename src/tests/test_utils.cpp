#include <string>

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