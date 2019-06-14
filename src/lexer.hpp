#include <string>
#include "token.hpp"
#include <iostream>

using namespace std;

class Lexer {
    private:
        string input;
        int position;
        int read_position;
        char ch;
        void skip_whitespace();
        string read_number();
        char peek_char();
    public:
        Lexer(string);
        void read_char();
        Token next_token();
        string read_identifier();
};

bool is_letter(char ch);
bool is_letter_or_digit(char ch);
bool is_digit(char ch);
