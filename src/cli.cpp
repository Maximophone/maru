#include "cli.hpp"
#include <set>
#include <iostream>

string lstrip(string str, char c){
    int i = 0;
    for(char& ch : str){
        if(ch!=c)
            break;
        i++;
    }
    return str.substr(i, str.length());
};

CliParser::CliParser(string help){
    this->help = help;
};

void CliParser::add_argument(string name, ArgType type, string help){
    ArgumentDef arg_def = {type, false, true, "", help};
    arguments_def[name] = arg_def;
};

void CliParser::add_optional(string name, ArgType type, string default_val, string help){
    ArgumentDef arg_def = {type, false, false, default_val, help};
    arguments_def[name] = arg_def;
};

void CliParser::add_positional(string name, ArgType type, string help){
    ArgumentDef arg_def = {type, false, true, "", help};
    positionals.push_back({name, arg_def});
};

void CliParser::add_positional_optional(string name, ArgType type, string default_val, string help){
    ArgumentDef arg_def = {type, false, false, default_val, help};
    positionals.push_back({name, arg_def});
};

void CliParser::add_flag(string name, string help){
    ArgumentDef arg_def = {"", true, false, "0", help};
    arguments_def[name] = arg_def;
};

void CliParser::add_alias(string alias, string name){
    if(arguments_def.find(name) == arguments_def.end()){
        throw "Can't create alias for non existing argument " + name;
    }
    aliases[alias] = name;
};

void CliParser::set_arg(Arguments &args, ArgumentDef arg_def, string name, string value){
    name = lstrip(name, '-');
    if(arg_def.type == INT_ARG){
        args.int_args[name] = stoi(value);
    } else if (arg_def.type == FLOAT_ARG) {
        args.float_args[name] = stof(value);
    } else if (arg_def.type == STRING_ARG) {
        args.string_args[name] = value;
    } else {
        throw "Unrecognised argument type " + arg_def.type;
    }
};

Arguments CliParser::parse(int argc, char* argv[]){
    Arguments args;
    set<string> found_args;
    int pos_index = 0;
    string arg_name;
    string arg_value;
    for(int i=1; i<argc; i++){
        arg_name = argv[i];
        if(aliases.find(arg_name) != aliases.end()){
            arg_name = aliases[arg_name];
        }
        ArgumentDef arg_def;
        if(arguments_def.find(arg_name) == arguments_def.end()){
            if(pos_index>=positionals.size()){
                throw "Argument not valid " + arg_name;
            }
            arg_value = arg_name;
            arg_name = positionals[pos_index].first;
            arg_def = positionals[pos_index].second;
            pos_index++;
            set_arg(args, arg_def, arg_name, arg_value);
            found_args.insert(arg_name);
            continue;
        }
        else {
            arg_def = arguments_def[arg_name];
        }
        found_args.insert(arg_name);
        if(arg_def.is_flag){
            args.flags[lstrip(arg_name, '-')] = true;
            continue;
        }
        i++;
        arg_value = argv[i];
        set_arg(args, arg_def, arg_name, arg_value);
    }

    for(auto const& x : arguments_def){
        string arg_name = x.first;
        if(found_args.find(arg_name) == found_args.end()){
            ArgumentDef arg_def = x.second;
            if(arg_def.required){
                throw "Missing required argument " + arg_name;
            }
            if(arg_def.is_flag){
                args.flags[lstrip(arg_name, '-')] = false;
                continue;
            }
            set_arg(args, arg_def, arg_name, arg_def.default_value);
        }
    }

    for(pair<string, ArgumentDef> x : positionals){
        string arg_name = x.first;
        if(found_args.find(arg_name) == found_args.end()){
            ArgumentDef arg_def = x.second;
            if(arg_def.required){
                throw "Missing required positional argument " + arg_name;
            }
            set_arg(args, arg_def, arg_name, arg_def.default_value);
        }
    }
    return args;
};