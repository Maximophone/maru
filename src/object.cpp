#include "object.hpp"
#include <string>
#include <functional>

using namespace std;

string Integer::inspect(){
    return to_string(value);
};

string Boolean::inspect(){
    return value?"true":"false";
};

string String::inspect(){
    return "\""+value+"\"";
};

string Array::inspect(){
    string el_string = "";
    for(int i = 0; i<elements.size(); i++){
        el_string += elements[i]->inspect();
        if(i<elements.size()-1)
            el_string += ",";
    }
    return "[" + el_string + "]";
};

string Null::inspect(){
    return "null";
};

string ReturnValue::inspect(){
    return value->inspect();
};

string Error::inspect(){
    return "ERROR: " + message;
};

Object* Environment::get(string name, bool& ok){
    Object* obj = store[name];
    ok = obj!=0;
    if(!ok && outer != 0){
        return outer->get(name, ok);
    }
    return obj;
};

Object* Environment::set(string name, Object* value){
    store[name] = value;
    return value;
};

string Function::inspect(){
    string param_string = "";
    for(int i = 0; i<parameters.size(); i++){
        param_string += parameters[i]->to_string();
        if(i<parameters.size()-1)
            param_string += ",";
    }
    return "fn(" + param_string + "){" + body->to_string() + "}";
};

string Builtin::inspect(){
    return "builtin function";
};

HashKey hash_key(Boolean* b){
    return b->value?HashKey{b->type, 1}:HashKey{b->type, 0};
};

HashKey hash_key(Integer* i){
    return HashKey{i->type, i->value};
};

HashKey hash_key(String* s){
    hash<string> hasher;
    int h = hasher(s->value);
    return HashKey{s->type, h};
};