#include "repl.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
#include <string>
#include <iostream>

using namespace std;

const string PROMPT = ">> ";

void print_parser_errors(vector<string> errors){
    cout << "Parser Errors: \n";
    for(string msg : errors){
        cout << "\t" << msg << "\n";
    }
};

void start(){
    string line;
    Environment* env = new Environment();
    while(true){
        
        cout << PROMPT;
        getline(cin, line);

        if(line == "exit"){
            return;
        }

        Lexer* l = new Lexer(line);
        Parser* p = new Parser(l);
        Program* program = p->parse_program();

        if(p->errors.size() != 0){
            print_parser_errors(p->errors);
            continue;
        }

        Object* evaluated = eval(program, env);
        if(evaluated != 0 && evaluated->type!=NULL_OBJ){
            cout << evaluated->inspect();
            cout << "\n";
        }
    }
}