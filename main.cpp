#include "src/repl.hpp"
#include "src/object.hpp"
#include "src/evaluator.hpp"
#include "src/parser.hpp"
#include "src/cli.hpp"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

void start_repl(Environment* env){
    cout << "Welcome to MARU.\n";
    cout << "Type in your commands.\n";
    start(env);
};

Environment* run_program(string input){
    Environment* env = new Environment();
    Lexer* l = new Lexer(input);
    Parser* p = new Parser(l);
    Program* program = p->parse_program();
    if(p->errors.size() != 0){
        print_parser_errors(p->errors);
        cout << "Exiting.";
        return env;
    };
    Object* ret = eval(program, env);
    if(Error* err = dynamic_cast<Error*>(ret)){
        cout << err->inspect() << "\n";
    };
    return env;
};

int main(int argc, char *argv[]){
    CliParser parser;

    parser.add_positional_optional("file", STRING_ARG, "", "program read from maru script file");
    parser.add_flag("-i", "starts interactive repl after running script");
    parser.add_optional("--cmd", STRING_ARG, "", "maru program passed in as a string");
    parser.add_alias("-c", "--cmd");

    Arguments args = parser.parse(argc, argv);

    if(args.string_args["file"] != ""){
        ifstream file;
        file.open(args.string_args["file"]);
        string line;
        string input;
        if(file.is_open()){
            while(getline(file, line)){
                input += line + "\n";
            }
            file.close();
        }
        else {
            cout << "Unable to open the file.\n";
            return 1;
        }
        Environment* env = run_program(input);
        if(args.flags["i"]){
            start_repl(env);
        }
    } else if(args.string_args["cmd"] != ""){
        Environment* env = run_program(args.string_args["cmd"]);
        if(args.flags["i"]){
            start_repl(env);
        }
    } else {
        start_repl(0);
    }
};