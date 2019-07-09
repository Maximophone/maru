#include "builtins.hpp"
#include "evaluator.hpp"
#include "maru_std/c_libraries/std_utils.hpp"
#include <vector>

Object* to_str(vector<Object*> args){
    if(Error* err = not_one_arg(args)){return err;};
    Object* obj = args[0];
    string value;
    if(obj->type == INTEGER_OBJ){
        value = to_string(((Integer*)obj)->value);
    } else if(obj->type == FLOAT_OBJ){
        value = to_string(((Float*)obj)->value);
    } else if(obj->type == STRING_OBJ){
        value = ((String*)obj)->value;
    } else {
        return new_error("argument to 'to_str' not supported: " + obj->type);
    }
    return new String(value);
}

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
        cout << arg->inspect() << "\n";
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

Object* assert(vector<Object*> args){
    string message;
    if(args.size() == 2){
        if(Error* err = check_signature(args, {{BOOLEAN_OBJ}, {STRING_OBJ}})){return err;}
        String* message_obj = (String*) args[1];
        message = ": " + message_obj->value;
    } else{
        if(Error* err = check_signature(args, {{BOOLEAN_OBJ}})){return err;}
        message = "";
    }
    Boolean* condition = (Boolean*) args[0];
    if(condition->value){
        return NULL_;
    }
    return new_error("Assertion" + message);
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
    {"to_str", new Builtin(to_str)},
    {"assert", new Builtin(assert)},
};