#include "ast.hpp"

class Parser{
    private:
        Lexer *lexer;
        Token cur_token;
        Token peek_token;
        void next_token();
    public:
        Parser(Lexer*);
        Program* parse_program();
};