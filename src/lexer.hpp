#include <string>

using namespace std;

class Lexer {
    public:
        string input;
        Lexer(string);
};

Lexer::Lexer(string input){
    input = input;
}