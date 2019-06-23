#include "builtins.hpp"
#include "evaluator.hpp"
#include <vector>

Object* len(vector<Object*> args){
    if(args.size() != 1){
        return new_error("wrong number of arguments. got=" + to_string(args.size()) + ", want=1");
    }
    if(String* arg = dynamic_cast<String*>(args[0])){
        return new Integer(arg->value.length());
    }
    if(Array* arg = dynamic_cast<Array*>(args[0])){
        return new Integer(arg->elements.size());
    }
    return new_error("argument to 'len' not supported, got "+args[0]->type);
};

Object* append(vector<Object*> args){
    if(args.size() <= 1){
        return new_error("wrong number of arguments. got="+to_string(args.size())+", wanted>1");
    }
    bool first = true;
    Array* arr = 0;
    for(Object* arg : args){
        if(first){
            first = false;
            arr = dynamic_cast<Array*>(arg);
            if(arr == 0){
                return new_error("first argument must be array. got=" + arg->type);
            }
        }
        else{
            arr->elements.push_back(arg);
        }
    }
    return arr;
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

Object* printline(vector<Object*> args){
    print(args);
    cout << "\n";
};

map<string, Builtin*> builtins = {
    {"len", new Builtin(len)},
    {"append", new Builtin(append)},
    {"print", new Builtin(print)},
    {"printline", new Builtin(printline)},
    {"printl", new Builtin(printline)},
};