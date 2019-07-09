#include "std_utils.hpp"

Error* single_num_arg(vector<Object*> args, double &ret){
    Error* err = check_signature(args, {{INTEGER_OBJ, FLOAT_OBJ}});
    if(err != 0){return err;}
    ret = get_numeric_value(args[0]);
    return 0;
}

Error* not_one_arg(vector<Object*> args){
    if(args.size() != 1){
        return new_error("wrong number of arguments. got=" + to_string(args.size()) + ", want=1");
    }
    return 0;
}

Error* check_signature(vector<Object*> args, vector<set<ObjectType>> signature){
    if(args.size() != signature.size()){
        return new_error("wrong number of arguments. got=" + to_string(args.size()) + ", want=" + to_string(signature.size()));
    }
    for(int i = 0; i<args.size(); i++){
        Object* arg = args[i];
        set<ObjectType> types = signature[i];
        string types_string = "";
        for(string typ : types){
            types_string += typ + ", ";
        }
        if(types.find(arg->type) == types.end()){
            return new_error("argument number " + to_string(i+1) + " accepted types are (" + types_string + ") but got " + arg->type);
        }
    }
    return 0;
}