#include "object.hpp"
#include <string>

using namespace std;

string Integer::inspect(){
    return to_string(value);
};

string Boolean::inspect(){
    return value?"true":"false";
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
    if(obj==0)
        ok = false;
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