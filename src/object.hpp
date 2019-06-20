#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <map>

using namespace std;

typedef string ObjectType;

const ObjectType INTEGER_OBJ = "INTEGER";
const ObjectType BOOLEAN_OBJ = "BOOLEAN";
const ObjectType NULL_OBJ = "NULL";
const ObjectType RETURN_OBJ = "RETURN";
const ObjectType ERROR_OBJ = "ERROR";

class Object {
    public:
        ObjectType type;
        virtual string inspect() = 0;
};

class Integer: public Object {
    public:
        int value;
        string inspect();
        Integer(){type=INTEGER_OBJ;};
        Integer(int val):Integer(){value=val;};
};

class Boolean: public Object {
    public:
        bool value;
        string inspect();
        Boolean(){type=BOOLEAN_OBJ;};
        Boolean(bool val):Boolean(){value=val;};
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

class Error: public Object {
    public:
        string message;
        string inspect();
        Error(){type=ERROR_OBJ;};
        Error(string msg):Error(){message=msg;};
};

class Environment{
    private:
        map<string, Object*> store;
    public:
        Object* get(string, bool&);
        Object* set(string, Object*);
};

#endif