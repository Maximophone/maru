#include "object.hpp"
#include <string>
#include <functional>

using namespace std;

string Integer::inspect(){
    return to_string(value);
};

string Float::inspect(){
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

Environment::Environment(Environment* env){
    if(env == this || env->outer == this){
        throw "Environment self nesting";
    }
    outer = env;
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

void Environment::set_from(Environment* env){
    map<string, Object*> other_store = env->get_store();
    store.insert(other_store.begin(), other_store.end());
};

Environment* Environment::copy(){
    Environment* new_env = new Environment();
    new_env->outer = this;
    new_env->store.insert(store.begin(), store.end());
    return new_env;
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

HashKey hash_key(Hashable* h){
    if(String* s = dynamic_cast<String*>(h))
        return hash_key(s);
    if(Integer* i = dynamic_cast<Integer*>(h))
        return hash_key(i);
    if(Boolean* b = dynamic_cast<Boolean*>(h))
        return hash_key(b);
    return HashKey(ERROR_OBJ, 0);
};

string Hash::inspect(){
    string pairs_string = "";
    map<HashKey, HashPair>::iterator it;
    int i = 0;
    for(it=pairs.begin(); it!=pairs.end(); it++){
        Object* key = it->second.key;
        Object* value = it->second.value;
        pairs_string += key->inspect()+ ":" + value->inspect();
        if(i<pairs.size()-1){
            pairs_string += ",";
        }
        i++;
    }
    return "{"+pairs_string+"}";
};

string Class::inspect(){
    string attr_string = "";
    for(Identifier* attr : attributes){
        attr_string += attr->to_string() + ";";
    }
    string const_str = (constructor!=0)?constructor->inspect():"";
    return "class{" + attr_string + const_str + "}";
};

string ClassInstance::inspect(){
    string attr_string = "";
    for(Identifier* attr: attributes){
        if(this->env == 0){
            attr_string += attr->to_string() + ";";
        }
        else {
            bool ok = true;
            Object* value = this->env->get(attr->value, ok);
            if(!ok || value == 0){
                attr_string += attr->to_string() + ";";
            }
            else if(value->type == FUNCTION_OBJ){
                continue;
            }
            else {
                attr_string += attr->to_string() + "=" + value->inspect() + ";";
            }
        }
    }
    return "instance{" + attr_string + "}";
};

string NameSpace::inspect(){
    return "namespace";
};