#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <map>
#include "ast.hpp"


using namespace std;

typedef string ObjectType;
typedef pair<ObjectType, int> HashKey;

const ObjectType INTEGER_OBJ = "INTEGER";
const ObjectType BOOLEAN_OBJ = "BOOLEAN";
const ObjectType STRING_OBJ = "STRING";
const ObjectType ARRAY_OBJ = "ARRAY";
const ObjectType HASH_OBJ = "HASH";
const ObjectType NULL_OBJ = "NULL";
const ObjectType RETURN_OBJ = "RETURN";
const ObjectType BREAK_OBJ = "BREAK";
const ObjectType CONTINUE_OBJ = "CONTINUE";
const ObjectType ERROR_OBJ = "ERROR";
const ObjectType FUNCTION_OBJ = "FUNCTION";
const ObjectType BUILTIN_OBJ = "BUILTIN";
const ObjectType CLASS_OBJ = "CLASS";
const ObjectType INSTANCE_OBJ = "INSTANCE";
const ObjectType NAMESPACE_OBJ = "NAMESPACE";

class Environment;
class Object;
class Hashable;
class Boolean;
class Integer;
class String;
class Function;

class Object {
    public:
        ObjectType type;
        virtual string inspect() = 0;
};

HashKey hash_key(Boolean*);
HashKey hash_key(Integer*);
HashKey hash_key(String*);
HashKey hash_key(Hashable*);

typedef Object* (*builtin_function)(vector<Object*>);

class Hashable: public Object {};

class Integer: public Hashable {
    public:
        int value;
        string inspect();
        Integer(){type=INTEGER_OBJ;};
        Integer(int val):Integer(){value=val;};
};

class Boolean: public Hashable {
    public:
        bool value;
        string inspect();
        Boolean(){type=BOOLEAN_OBJ;};
        Boolean(bool val):Boolean(){value=val;};
};

class String: public Hashable {
    public:
        string value;
        string inspect();
        String(){type=STRING_OBJ;};
        String(string val):String(){value=val;};
};

class Array: public Object {
    public:
        vector<Object*> elements;
        string inspect();
        Array(){type=ARRAY_OBJ;};
        Array(vector<Object*> el):Array(){elements=el;};
};

class HashPair {
    public:
        Object* key;
        Object* value;
};

class Hash: public Object {
    public:
        map<HashKey, HashPair> pairs;
        string inspect();
        Hash(){type=HASH_OBJ;};
};

class Class: public Object {
    public:
        vector<Identifier*> attributes;
        Function* constructor;
        Environment* env;
        string inspect();
        Class(){type=CLASS_OBJ;};
};

class ClassInstance: public Object {
    public:
        vector<Identifier*> attributes;
        Environment* env;
        string inspect();
        ClassInstance(){type=INSTANCE_OBJ;};
};

class NameSpace: public Object {
    public:
        Environment* env;
        string inspect();
        NameSpace(){type=NAMESPACE_OBJ;};
};

class Null: public Object {
    public:
        string inspect();
        Null(){type=NULL_OBJ;};
};

class ReturnValue: public Object {
    public:
        Object* value;
        string inspect();
        ReturnValue(){type=RETURN_OBJ;};
        ReturnValue(Object* obj):ReturnValue(){value=obj;};
};

class Break: public Object {
    public:
        string inspect(){return "break";};
        Break(){type=BREAK_OBJ;};
};

class Continue: public Object {
    public:
        string inspect(){return "continue";};
        Continue(){type=CONTINUE_OBJ;};
};

class Error: public Object {
    public:
        string message;
        string inspect();
        Error(){type=ERROR_OBJ;};
        Error(string msg):Error(){message=msg;};
};

class Function: public Object {
    public:
        vector<Identifier*> parameters;
        BlockStatement* body;
        Environment* env;
        string inspect();
        Function(){type=FUNCTION_OBJ;};
};

class Builtin: public Object {
    public:
        builtin_function fn;
        string inspect();
        Builtin(builtin_function f){type=BUILTIN_OBJ; fn=f;};
};

class Environment{
    private:
        map<string, Object*> store;
        Environment* outer;
    public:
        Object* get(string, bool&);
        Object* set(string, Object*);
        map<string, Object*> get_store(){return store;};
        void set_from(Environment*);
        Environment(){outer=0;};
        Environment(Environment*);
        Environment* copy();
};

#endif