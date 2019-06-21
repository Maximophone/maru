#include "src/repl.hpp"
#include "src/object.hpp"
#include "src/evaluator.hpp"
#include "src/parser.hpp"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

void run_program(string input){
    cout << "running program \n";
    cout << input << "\n";
    Environment* env = new Environment();
    Lexer* l = new Lexer(input);
    cout << "parsing input \n";
    Parser* p = new Parser(l);
    Program* program = p->parse_program();
    if(p->errors.size() != 0){
        print_parser_errors(p->errors);
        cout << "Exiting.";
        return;
    }
    cout << "evaluating program \n";
    Object* evaluated = eval(program, env);

    if(evaluated != 0){
        cout << evaluated->inspect() << "\n";
    }
};

int main(int argc, char *argv[]){
    if(argc >= 2){
        string file_path = argv[1];
        ifstream file;
        file.open(file_path);
        string line;
        string input;
        if(file.is_open()){
            cout << "reading file \n";
            while(getline(file, line)){
                input += line + "\n";
            }
            file.close();
        }
        else {
            cout << "Unable to open the file.\n";
            return 1;
        }
        run_program(input);
    }else{
        cout << "Welcome to MARU.\n";
        cout << "Type in your commands.\n";
        start();
    }
};