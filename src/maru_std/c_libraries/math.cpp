#include "math.hpp"
#include "std_utils.hpp"
#include <math.h>

Object* cosine_(vector<Object*> args){
    double val = 0;
    if(Error* err = single_num_arg(args, val)){ return err ;}
    return new Float(cos(val));
};
CFunction* cosine = new CFunction(cosine_);

Object* sine_(vector<Object*> args){
    double val = 0;
    if(Error* err = single_num_arg(args, val)){ return err ;}
    return new Float(sin(val));
};
CFunction* sine = new CFunction(sine_);

Environment* math_env(){
    return new Environment({
        {"cos", cosine},
        {"sin", sine},
        {"pi", new Float(M_PI)},
        {"e", new Float(M_E)},
    });
};