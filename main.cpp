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

void run_lexer(string input){
    Lexer* l = new Lexer(input);
    Token tok;
    do{
        tok = l->next_token();
        cout << "(" << tok.type << "," << tok.literal << ") ";
    } while (tok.type != END);
    cout << "\n";
};

void run_parser(string input){
    Lexer* l = new Lexer(input);
    Parser* p = new Parser(l);
    Program* program = p->parse_program();
    if(p->errors.size() != 0){
        print_parser_errors(p->errors);
        return;
    }
    cout << program->to_string() << "\n";
};

Environment* run_program(string input, bool show_result, bool test){
    Environment* env = new Environment(test);
    Lexer* l = new Lexer(input);
    Parser* p = new Parser(l);
    Program* program = p->parse_program();
    if(p->errors.size() != 0){
        print_parser_errors(p->errors);
        return env;
    };
    Object* ret = eval(program, env);
    if(Error* err = dynamic_cast<Error*>(ret)){
        cout << err->inspect() << "\n";
    } else {
        if(show_result && !(ret->type==NULL_OBJ)){
            cout << ret->inspect() << "\n";
        }
        if(test){
            bool ok = true;
            cout << env->get(TEST_RESULTS_ENV_VAR, ok)->inspect() << "\n";
        }
    }
    return env;
};

void process_string(string code, Arguments args){
    if(args.flags["lexer-only"]){
        run_lexer(code);
    } else if (args.flags["parser-only"]) {
        run_parser(code);
    } else {
        Environment* env = run_program(code, args.flags["show-last"], args.flags["test"]);
        if(args.flags["i"]){
            start_repl(env);
        }
    }
};

int main(int argc, char *argv[]){
    CliParser parser("The Maru Programming Language.");

    parser.add_positional_optional("file", STRING_ARG, "", "program read from maru script file");
    parser.add_flag("--test", "starts execution with tests");
    parser.add_alias("-t", "--test");
    parser.add_flag("-i", "starts interactive repl after running script");
    parser.add_optional("--cmd", STRING_ARG, "", "maru program passed in as a string");
    parser.add_alias("-c", "--cmd");
    parser.add_flag("--show-last", "prints the value of the last statement before exiting");
    parser.add_alias("-sl", "--show-last");
    parser.add_flag("--lexer-only", "Only runs lexer and shows output");
    parser.add_alias("-l", "--lexer-only");
    parser.add_flag("--parser-only", "Only runs parser and shows output");
    parser.add_alias("-p", "--parser-only");

    Arguments args = parser.parse(argc, argv);

    if(args.provided("file")){
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
        process_string(input, args);
    } else if(args.provided("--cmd")){
        process_string(args.string_args["cmd"], args);
    } else {
        if(args.flags["test"]){
            start_repl(new Environment(true));
        }
        else{
            start_repl(0);
        }
    }
};