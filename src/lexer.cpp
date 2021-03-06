#include "lexer.hpp"

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
    Token tok;
    
    while(is_whitespace(ch) || is_line_comment_tag(ch)){
        if(is_line_comment_tag(ch)){
            read_comment();
        }
        else{
            read_char();
        }
    }
    
    string sch(1, ch);
    switch(ch){
        case '=':
            if(peek_char() == '='){
                read_char();
                tok = Token{EQUAL, "=="};
            }
            else {
                tok = Token{ASSIGN, sch};
            }
            break;
        case ';':
            tok = Token{SEMICOLON, sch};
            break;
        case ':':
            tok = Token{COLON, sch};
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
        case '[':
            tok = Token{LBRACKET, sch};
            break;
        case ']':
            tok = Token{RBRACKET, sch};
            break;
        case ',':
            tok = Token{COMMA, sch};
            break;
        case '.':
            tok = Token{DOT, sch};
            break;
        case '+':
            tok = Token{PLUS, sch};
            break;
        case '-':
            tok = Token{MINUS, sch};
            break;
        case '*':
            tok = Token{ASTERIX, sch};
            break;
        case '!':
            if(peek_char() == '='){
                read_char();
                tok = Token{NOT_EQUAL, "!="};
            } else {
                tok = Token{BANG, sch};
            }
            break;
        case '/':
            tok = Token{SLASH, sch};
            break;
        case '<':
            tok = Token{LT, sch};
            break;
        case '>':
            tok = Token{GT, sch};
            break;
        case 0:
            tok = Token{END, ""};
            break;
        case '"':
            tok = Token{STRING, read_string()};
            break;
        default:
            if(is_letter(ch)){
                tok.literal = read_identifier();
                tok.type = lookup_ident(tok.literal);
                return tok;
            }
            else if(is_digit(ch)){
                bool is_float = false;
                tok.literal = read_number(is_float);
                tok.type = is_float?FLOAT:INT;
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

string Lexer::read_number(bool& is_float){
    int prev_pos = position;
    while(is_digit(ch) || ch=='.'){
        if(ch=='.'){
            is_float = true;
        }
        read_char();
    };
    return input.substr(prev_pos, position-prev_pos);
};

string Lexer::read_string(){
    read_char();
    int prev_pos = position;
    while(ch!='"' && ch != 0){
        read_char();
    }
    return input.substr(prev_pos, position-prev_pos);
};

void Lexer::read_comment(){
    while(ch!=0 and ch!='\n'){
        read_char();
    }
};

char Lexer::peek_char(){
    if(read_position >= input.length()){
        return 0;
    } else {
        return input[read_position];
    };
};

bool is_letter(char ch){
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
};

bool is_digit(char ch){
    return '0' <= ch && ch <= '9';
}

bool is_letter_or_digit(char ch){
    return is_letter(ch) || is_digit(ch);
};

bool is_line_comment_tag(char ch){
    return ch == COMMENT;
};

bool is_whitespace(char ch){
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
};