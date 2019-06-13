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
    public:
        Lexer(string);
        void read_char();
        Token next_token();
        string read_identifier();
};

bool is_letter(char ch);
bool is_letter_or_digit(char ch);

Lexer::Lexer(string npt){
    input = npt;
    position = 0;
    read_position = 0;
    read_char();
};

void Lexer::read_char(){
    if(read_position >= input.length()){
        ch = 0;
    } else {
        ch = input[read_position];
    };
    position = read_position;
    read_position++;
};

Token Lexer::next_token(){
    cout << ch;
    Token tok;
    string sch(1, ch);

    skip_whitespace();

    switch(ch){
        case '=':
            tok = Token{ASSIGN, sch};
            break;
        case ';':
            tok = Token{SEMICOLON, sch};
            break;
        case '(':
            tok = Token{LPAREN, sch};
            break;
        case ')':
            tok = Token{RPAREN, sch};
            break;
        case '{':
            tok = Token{LBRACE, sch};
            break;
        case '}':
            tok = Token{RBRACE, sch};
            break;
        case ',':
            tok = Token{COMMA, sch};
            break;
        case '+':
            tok = Token{PLUS, sch};
            break;
        case 0:
            tok = Token{END, ""};
            break;
        default:
            if(is_letter(ch)){
                tok.literal = read_identifier();
                tok.type = lookup_ident(tok.literal);
                return tok;
            } else {
                tok = Token{ILLEGAL, sch};
            }
    }
    read_char();
    return tok;
};

string Lexer::read_identifier(){
    int prev_pos = position;
    while(is_letter_or_digit(ch)){
        read_char();
    };
    return input.substr(prev_pos, position-prev_pos);
};

void Lexer::skip_whitespace(){
    while(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'){
        read_char();
    }
};

bool is_letter(char ch){
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
};

bool is_letter_or_digit(char ch){
    return is_letter(ch) || ('1' <= ch && ch <= '9') || ch == '0';
};