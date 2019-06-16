#include "parser.hpp"
#include "lexer.hpp"

Parser::Parser(Lexer *l){
    lexer = l;
    next_token();
    next_token();

    prefix_parse_funcs[IDENT] = &Parser::parse_identifier;
    prefix_parse_funcs[INT] = &Parser::parse_integer_literal;
    prefix_parse_funcs[BANG] = &Parser::parse_prefix_expression;
    prefix_parse_funcs[MINUS] = &Parser::parse_prefix_expression;
};

void Parser::next_token(){
    cur_token = peek_token;
    peek_token = lexer->next_token();
};

Program* Parser::parse_program(){
    Program *program = new Program();
    program->statements = {};
    while(cur_token.type != END){
        Statement* stmt = parse_statement();
        if(stmt != 0){
            program->statements.push_back(stmt);
        }
        next_token();
    }
    return program;
};

Statement* Parser::parse_statement(){
    if(cur_token.type==LET)
        return parse_let_statement();
    if(cur_token.type==RETURN)
        return parse_return_statement();
    return parse_expression_statement();
};

LetStatement* Parser::parse_let_statement(){
    LetStatement* stmt = new LetStatement();
    stmt->token = cur_token;
    if(!expect_peek(IDENT)){
        return 0;
    }
    stmt->name = new Identifier();
    stmt->name->token = cur_token;
    stmt->name->value = cur_token.literal;
    if(!expect_peek(ASSIGN)){
        return 0;
    }
    // TODO: We're skipping the expressions until we
    // encounter a semicolon
    while(!cur_token_is(SEMICOLON)){
        next_token();
    }
    return stmt;
};

ReturnStatement* Parser::parse_return_statement(){
    ReturnStatement* stmt = new ReturnStatement();
    stmt->token = cur_token;
    next_token();
    // TODO: We're skipping the expressions until we
    // encounter a semicolon
    while(!expect_peek(SEMICOLON)){
        next_token();
    }
    return stmt;
}

ExpressionStatement* Parser::parse_expression_statement(){
    ExpressionStatement* stmt = new ExpressionStatement();
    stmt->token = cur_token;
    stmt->expression = parse_expression(LOWEST);
    if(peek_token_is(SEMICOLON)){
        next_token();
    }
    return stmt;
};

Expression* Parser::parse_expression(int precedence){
    prefix_parse_func prefix = prefix_parse_funcs[cur_token.type];
    if(prefix == 0){
        no_prefix_parse_func_error(cur_token.type);
        return 0;
    }
    Expression* left_exp = (this->*prefix)();
    return left_exp;
};

Expression* Parser::parse_identifier(){
    Identifier* ident = new Identifier();
    ident->token = cur_token;
    ident->value = cur_token.literal;
    return ident;
};

Expression* Parser::parse_integer_literal(){
    IntegerLiteral* lit = new IntegerLiteral();
    lit->token = cur_token;
    try{
        lit->value = stoi(cur_token.literal);
    } catch(invalid_argument e) {
        errors.push_back("could not parse " + cur_token.literal + " as an integer");
        return 0;
    }
    return lit;
};

Expression* Parser::parse_prefix_expression(){
    PrefixExpression* exp = new PrefixExpression();
    exp->token = cur_token;
    exp->op = cur_token.literal;
    next_token();
    exp->right = parse_expression(PREFIX);
    return exp;
};

void Parser::no_prefix_parse_func_error(TokenType t){
    string message = "no prefix parse function for " + t + " found";
    errors.push_back(message);
};

bool Parser::cur_token_is(TokenType t){
    return cur_token.type == t;
};

bool Parser::peek_token_is(TokenType t){
    return peek_token.type == t;
};

bool Parser::expect_peek(TokenType t){
    if(peek_token_is(t)){
        next_token();
        return true;
    } else {
        peek_error(t);
        return false;
    }
};

void Parser::peek_error(TokenType t){
    string message = "expected next token to be " + 
        t + " but got " + peek_token.type + " instead";
    errors.push_back(message);
};
