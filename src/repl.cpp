#include "repl.hpp"
#include <iostream>
#include "parser.hpp"
#include <string>

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
    while(true){
        
        cout << PROMPT;
        getline(cin, line);

        if(line == ""){
            return;
        }

        Lexer* l = new Lexer(line);
        Parser* p = new Parser(l);
        Program* program = p->parse_program();

        if(p->errors.size() != 0){
            print_parser_errors(p->errors);
            continue;
        }

        cout << program->to_string();
        cout << "\n";
    }
}