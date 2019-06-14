#include <iostream>
#include "lexer.hpp"
#include <string>

using namespace std;

const string PROMPT = ">> ";

void start(){
    string line;
    while(true){
        
        cout << PROMPT;
        getline(cin, line);

        if(line == ""){
            return;
        }

        Lexer l(line);

        for(Token tok = l.next_token(); tok.type != END; tok = l.next_token()){
            cout << "(" << tok.type << "," << tok.literal << ")";
        }
        cout << "\n";

    }
}