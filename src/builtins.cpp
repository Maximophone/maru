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
}

map<string, Builtin*> builtins = {
    {"len", new Builtin(len)}
};