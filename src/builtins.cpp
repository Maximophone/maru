#include "builtins.hpp"
#include "evaluator.hpp"
#include <vector>

Object* len(vector<Object*> args){
    if(args.size() != 1){
        return new_error("wrong number of arguments. got=" + to_string(args.size()) + ", want=1");
    }
    String* arg = dynamic_cast<String*>(args[0]);
    if(arg == 0){
        return new_error("argument to 'len' not supported, got "+args[0]->type);
    }
    return new Integer(arg->value.length());
};

Object* print(vector<Object*> args){
    for(Object* arg : args){
        String* arg_str = dynamic_cast<String*>(arg);
        if(arg_str == 0){
            return new_error("arguments to 'print' must be STRING, got "+arg->type);
        }
        cout << arg_str->value;
    }
    return NULL_;
};

map<string, Builtin*> builtins = {
    {"len", new Builtin(len)},
    {"print", new Builtin(print)},
};