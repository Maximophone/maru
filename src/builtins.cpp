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

Object* range(vector<Object*> args){
    if(args.size() < 1 || args.size() > 2){
        return new_error("wrong number of arguments. range takes 1 or 2 but got=" + to_string(args.size()));
    }
    for(Object* arg : args){
        if(!dynamic_cast<Integer*>(arg))
            return new_error("all arguments to range should be INTEGER. got " + arg->type);
    }
    int n_min = 0;
    int n_max = 0;
    if(args.size() == 1){
        n_max = dynamic_cast<Integer*>(args[0])->value;
    }
    if(args.size() == 2){
        n_min = dynamic_cast<Integer*>(args[0])->value;
        n_max = dynamic_cast<Integer*>(args[1])->value;
    }
    vector<Object*> elements;
    for(int i = n_min; i<n_max; i++){
        elements.push_back(new Integer(i));
    }
    return new Array(elements);
}

Object* repr(vector<Object*> args){
    for(Object* arg: args){
        cout << arg->inspect();
    }
    return NULL_;
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
    Object* ret = print(args);
    if(!is_error(ret))
        cout << "\n";
    return ret;
};

Object* env(vector<Object*> args){
    if(args.size() != 1){
        return new_error("wrong number of arguments. got=" + to_string(args.size()) + ", want=1");
    }
    if(NameSpace* arg = dynamic_cast<NameSpace*>(args[0])){
        map<string, Object*> store = arg->env->get_store();
        Array* arr = new Array();
        for(map<string, Object*> ::iterator it = store.begin(); it != store.end(); ++it) {
            arr->elements.push_back(new String(it->first));
        }
        return arr;
    }
    return new_error("argument to 'env' not supported, got "+args[0]->type);
};

Object* type_(vector<Object*> args){
    if(args.size() != 1){
        return new_error("wrong number of arguments. got=" + to_string(args.size()) + ", want=1");
    }
    return new String(args[0]->type);
};

map<string, Builtin*> builtins = {
    {"len", new Builtin(len)},
    {"append", new Builtin(append)},
    {"range", new Builtin(range)},
    {"repr", new Builtin(repr)},
    {"print", new Builtin(print)},
    {"printline", new Builtin(printline)},
    {"printl", new Builtin(printline)},
    {"env", new Builtin(env)},
    {"type", new Builtin(type_)},
};