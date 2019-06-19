#ifndef OBJECT_H
#define OBJECT_H

#include <string>

using namespace std;

typedef string ObjectType;

const ObjectType INTEGER_OBJ = "INTEGER";
const ObjectType BOOLEAN_OBJ = "BOOLEAN";
const ObjectType NULL_OBJ = "NULL";

class Object {
    public:
        ObjectType type;
        virtual string inspect() = 0;
};

class Integer: public Object {
    public:
        int value;
        ObjectType type = INTEGER_OBJ;
        string inspect();
        Integer(){};
        Integer(int val){value=val;};
};

class Boolean: public Object {
    public:
        bool value;
        ObjectType type = BOOLEAN_OBJ;
        string inspect();
        Boolean(bool val){value=val;};
};

class Null: public Object {
    public:
        ObjectType type = NULL_OBJ;
        string inspect();
};

class ReturnValue: public Object{
    public:
        Object* value;
        string inspect();
        ReturnValue(Object* obj){value=obj;};
};

#endif