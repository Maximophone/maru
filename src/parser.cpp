#include "parser.hpp"
#include "lexer.hpp"

map<TokenType, int> precedences = {
    {EQUAL, EQUALS},
    {NOT_EQUAL, EQUALS},
    {LT, LESSGREATER},
    {GT, LESSGREATER},
    {PLUS, SUM},
    {MINUS, SUM},
    {SLASH, PRODUCT},
    {ASTERIX, PRODUCT},
    {LPAREN, CALL},
    {LBRACKET, INDEX},
    {ASSIGN, ASSIGNMENT},
    {DOT, ATTR},
};

Parser::Parser(Lexer *l)
{
    lexer = l;
    next_token();
    next_token();

    prefix_parse_funcs[IDENT] = &Parser::parse_identifier;
    prefix_parse_funcs[INT] = &Parser::parse_integer_literal;
    prefix_parse_funcs[FLOAT] = &Parser::parse_float_literal;
    prefix_parse_funcs[STRING] = &Parser::parse_string_literal;
    prefix_parse_funcs[TRU] = &Parser::parse_boolean_literal;
    prefix_parse_funcs[FALS] = &Parser::parse_boolean_literal;
    prefix_parse_funcs[BANG] = &Parser::parse_prefix_expression;
    prefix_parse_funcs[MINUS] = &Parser::parse_prefix_expression;
    prefix_parse_funcs[LPAREN] = &Parser::parse_grouped_expression;
    prefix_parse_funcs[IF] = &Parser::parse_if_expression;
    prefix_parse_funcs[FOR] = &Parser::parse_for_expression;
    prefix_parse_funcs[WHILE] = &Parser::parse_while_expression;
    prefix_parse_funcs[FUNCTION] = &Parser::parse_function_literal;
    prefix_parse_funcs[LBRACKET] = &Parser::parse_array_literal;
    prefix_parse_funcs[LBRACE] = &Parser::parse_hash_literal;
    prefix_parse_funcs[CLASS] = &Parser::parse_class_literal;

    infix_parse_funcs[PLUS] = &Parser::parse_infix_expression;
    infix_parse_funcs[MINUS] = &Parser::parse_infix_expression;
    infix_parse_funcs[SLASH] = &Parser::parse_infix_expression;
    infix_parse_funcs[ASTERIX] = &Parser::parse_infix_expression;
    infix_parse_funcs[EQUAL] = &Parser::parse_infix_expression;
    infix_parse_funcs[NOT_EQUAL] = &Parser::parse_infix_expression;
    infix_parse_funcs[LT] = &Parser::parse_infix_expression;
    infix_parse_funcs[GT] = &Parser::parse_infix_expression;
    infix_parse_funcs[LPAREN] = &Parser::parse_call_expression;
    infix_parse_funcs[LBRACKET] = &Parser::parse_index_expression;
    infix_parse_funcs[ASSIGN] = &Parser::parse_assign_expression;
    infix_parse_funcs[DOT] = &Parser::parse_access_expression;
};

void Parser::next_token()
{
    cur_token = peek_token;
    peek_token = lexer->next_token();
};

Program *Parser::parse_program()
{
    Program *program = new Program();
    program->statements = {};
    while (cur_token.type != END)
    {
        Statement *stmt = parse_statement();
        if (stmt != 0)
        {
            program->statements.push_back(stmt);
        }
        next_token();
    }
    return program;
};

Statement *Parser::parse_statement()
{
    if (cur_token.type == LET)
        return parse_let_statement();
    if (cur_token.type == RETURN)
        return parse_return_statement();
    if (cur_token.type == BREAK)
        return parse_continue_break_statement(BREAK);
    if (cur_token.type == CONTINUE)
        return parse_continue_break_statement(CONTINUE);
    if (cur_token.type == INCLUDE)
        return parse_include_statement();
    return parse_expression_statement();
};

LetStatement *Parser::parse_let_statement()
{
    LetStatement *stmt = new LetStatement();
    stmt->token = cur_token;
    if (!expect_peek(IDENT))
    {
        return 0;
    }
    stmt->name = new Identifier();
    stmt->name->token = cur_token;
    stmt->name->value = cur_token.literal;
    if (!expect_peek(ASSIGN))
    {
        return 0;
    }
    next_token();
    stmt->value = parse_expression(LOWEST);
    if(peek_token_is(SEMICOLON))
        next_token();
    return stmt;
};

ReturnStatement *Parser::parse_return_statement()
{
    ReturnStatement *stmt = new ReturnStatement();
    stmt->token = cur_token;
    next_token();
    stmt->return_value = parse_expression(LOWEST);
    if(peek_token_is(SEMICOLON))
        next_token();
    return stmt;
};

Statement *Parser::parse_continue_break_statement(TokenType t){
    Statement* stmt;
    if(t==BREAK){
        stmt = new BreakStatement();
        stmt->token = cur_token;
    }
    if(t==CONTINUE){
        stmt = new ContinueStatement();
        stmt->token = cur_token;
    }
    if(peek_token_is(SEMICOLON))
        next_token();
    return stmt;
};

IncludeStatement *Parser::parse_include_statement(){
    IncludeStatement* stmt = new IncludeStatement();
    stmt->token = cur_token;
    if(!expect_peek(STRING)){
        return 0;
    }
    Expression* exp = parse_string_literal();
    StringLiteral* string_lit = dynamic_cast<StringLiteral*>(exp);
    if(string_lit == 0){
        throw "parse_string_literal returned expression that cannot be converted to a StringLiteral";
    }
    stmt->path = string_lit;

    if(peek_token_is(SEMICOLON)){
        next_token();
        stmt->namespace_ = 0;
        return stmt;
    }

    if(!expect_peek(AS)){
        return 0;
    }
    if(!expect_peek(IDENT)){
        return 0;
    }

    Expression* ident_exp = parse_identifier();
    Identifier* ident = dynamic_cast<Identifier*>(ident_exp);
    if(ident == 0){
        throw "parse_identifier returned expression that cannot be converted to an Identifier";
    }

    stmt->namespace_ = ident;

    if(peek_token_is(SEMICOLON)){
        next_token();
    }
    return stmt;
};

ExpressionStatement *Parser::parse_expression_statement()
{
    ExpressionStatement *stmt = new ExpressionStatement();
    stmt->token = cur_token;
    Expression *exp = parse_expression(LOWEST);
    if(exp == 0){
        return 0;
    }
    stmt->expression = exp;
    if (peek_token_is(SEMICOLON))
    {
        next_token();
    }
    return stmt;
};

Expression *Parser::parse_expression(int precedence)
{
    // cout << "Parsing Expression \n";
    // cout << "    Current precedence " << precedence << "\n";
    // cout << "    Next tokens (" << cur_token.literal << ", " << peek_token.literal << ")\n";
    prefix_parse_func prefix = prefix_parse_funcs[cur_token.type];
    if (prefix == 0)
    {
        // cout << "No prefix parse function found. Error.";
        no_prefix_parse_func_error(cur_token);
        return 0;
    }
    Expression *left_exp = (this->*prefix)();

    while (!peek_token_is(SEMICOLON) && precedence < peek_precedence())
    {
        // cout << "Checking infix parse functions \n";
        infix_parse_func infix = infix_parse_funcs[peek_token.type];
        if (infix == 0)
        {
            // cout << "No infix function found for type " << cur_token.type << "\n";
            return left_exp;
        };
        next_token();
        left_exp = (this->*infix)(left_exp);
    }
    return left_exp;
};

Expression *Parser::parse_identifier()
{
    Identifier *ident = new Identifier();
    ident->token = cur_token;
    ident->value = cur_token.literal;
    return ident;
};

Expression *Parser::parse_integer_literal()
{
    IntegerLiteral *lit = new IntegerLiteral();
    lit->token = cur_token;
    try
    {
        lit->value = stoi(cur_token.literal);
    }
    catch (invalid_argument e)
    {
        errors.push_back("could not parse " + cur_token.literal + " as an integer");
        return 0;
    }
    return lit;
};

Expression *Parser::parse_float_literal()
{
    FloatLiteral* lit = new FloatLiteral();
    lit->token = cur_token;
    try
    {
        lit->value = stod(cur_token.literal);
    }
    catch(invalid_argument e)
    {
        errors.push_back("could not parse " + cur_token.literal + " as a float");
        return 0;
    }
    return lit;
};

Expression *Parser::parse_boolean_literal()
{
    BooleanLiteral *lit = new BooleanLiteral();
    lit->token = cur_token;
    lit->value = cur_token_is(TRU);
    return lit;
};

Expression *Parser::parse_string_literal()
{
    StringLiteral *lit = new StringLiteral();
    lit->token = cur_token;
    lit->value = cur_token.literal;
    return lit;
};

Expression *Parser::parse_array_literal()
{
    ArrayLiteral* array = new ArrayLiteral();
    bool ok = true;
    array->elements = parse_expression_list(RBRACKET, ok);
    if(!ok)
        return 0;
    return array;
};

Expression *Parser::parse_hash_literal()
{
    HashLiteral* hash = new HashLiteral();
    while(!peek_token_is(RBRACE)){
        next_token();
        Expression* key = parse_expression(LOWEST);
        if(!expect_peek(COLON)){
            return 0;
        }
        next_token();
        Expression* value = parse_expression(LOWEST);
        hash->pairs[key] = value;
        if(!peek_token_is(RBRACE) && !expect_peek(COMMA)){
            return 0;
        }
    }
    if(!expect_peek(RBRACE)){
        return 0;
    }
    return hash;
};

Expression *Parser::parse_class_literal()
{
    ClassLiteral *lit = new ClassLiteral();
    lit->token = cur_token;
    if(peek_token_is(LPAREN)){
        next_token();
        if(!expect_peek(IDENT))
            return 0;
        lit->parent = (Identifier*) parse_identifier();
        if(!expect_peek(RPAREN))
            return 0;
    }
    if(!expect_peek(LBRACE))
        return 0;
    lit->body = parse_block_statement();
    return lit;
};

Expression *Parser::parse_function_literal()
{
    FunctionLiteral *lit = new FunctionLiteral();
    lit->token = cur_token;
    if (!expect_peek(LPAREN))
    {
        return 0;
    }
    bool ok = true;
    lit->parameters = parse_function_parameters(ok);
    if(!ok){
        return 0;
    }
    if (!expect_peek(LBRACE))
    {
        return 0;
    }
    lit->body = parse_block_statement();
    return lit;
};

vector<Identifier *> Parser::parse_function_parameters(bool& ok)
{
    vector<Identifier *> identifiers = {};
    if (peek_token_is(RPAREN))
    {
        next_token();
        return identifiers;
    }
    next_token();
    Identifier* ident = new Identifier();
    ident->token = cur_token;
    ident->value = cur_token.literal;
    identifiers.push_back(ident);
    while (peek_token_is(COMMA))
    {
        next_token(); next_token();
        ident = new Identifier();
        ident->token = cur_token;
        ident->value = cur_token.literal;
        identifiers.push_back(ident);
    }
    if(!expect_peek(RPAREN)){
        ok = false;
        return {};
    }
    return identifiers;
};

Expression* Parser::parse_call_expression(Expression* callable){
    CallExpression* exp = new CallExpression();
    exp->token = cur_token;
    exp->callable = callable;
    bool ok = true;
    exp->arguments = parse_expression_list(RPAREN, ok);
    if(!ok){
        return 0;
    }
    return exp;
};

Expression* Parser::parse_index_expression(Expression* left){
    IndexExpression* exp = new IndexExpression();
    exp->token = cur_token;
    exp->left = left;
    next_token();
    exp->index = parse_expression(LOWEST);
    if(!expect_peek(RBRACKET))
        return 0;
    return exp;
};

Expression* Parser::parse_access_expression(Expression* left){
    AccessExpression* exp = new AccessExpression();
    exp->token = cur_token;
    exp->object = left;
    if(!expect_peek(IDENT))
        return 0;
    exp->attribute = (Identifier*)parse_identifier();
    return exp;
};

vector<Expression*> Parser::parse_expression_list(TokenType end, bool& ok){
    vector<Expression*> args = {};
    if(peek_token_is(end)){
        next_token();
        return args;
    }
    next_token();
    args.push_back(parse_expression(LOWEST));
    while(peek_token_is(COMMA)){
        next_token(); next_token();
        args.push_back(parse_expression(LOWEST));
    }
    if(!expect_peek(end)){
        ok = false;
        return {};
    }
    return args;
};

Expression* Parser::parse_if_expression()
{
    IfExpression *exp = new IfExpression();
    exp->token = cur_token;

    if (!expect_peek(LPAREN))
    {
        return 0;
    }

    next_token();

    exp->condition = parse_expression(LOWEST);

    if (!expect_peek(RPAREN))
    {
        return 0;
    }

    if (!expect_peek(LBRACE))
    {
        return 0;
    }

    exp->consequence = parse_block_statement();

    if (peek_token_is(ELSE))
    {
        next_token();

        if (!expect_peek(LBRACE))
        {
            return 0;
        }

        exp->alternative = parse_block_statement();
    }

    return exp;
};

Expression* Parser::parse_for_expression()
{
    ForExpression *exp = new ForExpression();
    exp->token = cur_token;

    if(!expect_peek(LPAREN))
        return 0;

    if(!expect_peek(IDENT))
        return 0;

    exp->iterator = dynamic_cast<Identifier*>(parse_identifier());

    if(!expect_peek(IN))
        return 0;
    next_token();

    exp->iterated = parse_expression(LOWEST);

    if(!expect_peek(RPAREN))
        return 0;

    if(!expect_peek(LBRACE))
        return 0;

    exp->body = parse_block_statement();

    return exp;
};

Expression* Parser::parse_while_expression()
{
    WhileExpression *exp = new WhileExpression();
    exp->token = cur_token;

    if(!expect_peek(LPAREN))
        return 0;
    next_token();
    exp->condition = parse_expression(LOWEST);

    if(!expect_peek(RPAREN))
        return 0;
    if(!expect_peek(LBRACE))
        return 0;
    
    exp->body = parse_block_statement();

    return exp;
};

BlockStatement *Parser::parse_block_statement()
{
    BlockStatement *block = new BlockStatement();
    block->token = cur_token;

    next_token();

    while (!cur_token_is(RBRACE))
    {
        Statement *stmt = parse_statement();
        if (stmt != 0)
        {
            block->statements.push_back(stmt);
        }
        else {
            return 0;
        }
        next_token();
    }
    return block;
};

Expression *Parser::parse_prefix_expression()
{
    PrefixExpression *exp = new PrefixExpression();
    exp->token = cur_token;
    exp->op = cur_token.literal;
    next_token();
    exp->right = parse_expression(PREFIX);
    return exp;
};

Expression *Parser::parse_infix_expression(Expression *left)
{
    // cout << "Parsing infix expression\n";
    InfixExpression *exp = new InfixExpression();
    exp->token = cur_token;
    exp->op = cur_token.literal;
    exp->left_value = left;
    int precedence = cur_precedence();
    // cout << "   Current precedence " << precedence << "\n";
    next_token();
    exp->right_value = parse_expression(precedence);
    return exp;
};

Expression* Parser::parse_assign_expression(Expression* left){
    AssignExpression* exp = new AssignExpression();
    exp->token = cur_token;
    exp->name = left;
    int precedence = cur_precedence();
    next_token();
    exp->value = parse_expression(precedence);
    return exp;
};

Expression *Parser::parse_grouped_expression()
{
    next_token();

    Expression *exp = parse_expression(LOWEST);

    if (!expect_peek(RPAREN))
    {
        return 0;
    }
    return exp;
};

void Parser::no_prefix_parse_func_error(Token t)
{
    string message = "no prefix parse function for (" + t.type + ":" + t.literal + ") found";
    errors.push_back(message);
};

bool Parser::cur_token_is(TokenType t)
{
    return cur_token.type == t;
};

bool Parser::peek_token_is(TokenType t)
{
    return peek_token.type == t;
};

bool Parser::expect_peek(TokenType t)
{
    if (peek_token_is(t))
    {
        next_token();
        return true;
    }
    else
    {
        peek_error(t);
        return false;
    }
};

void Parser::peek_error(TokenType t)
{
    string message = "expected next token to be " +
                     t + " but got " + peek_token.type + " instead";
    errors.push_back(message);
};

int Parser::cur_precedence()
{
    return precedences[cur_token.type];
};

int Parser::peek_precedence()
{
    // cout << "Peeking precedence for token " << peek_token.type << "\n";
    // cout << "Found " << precedences[peek_token.type] << "\n";
    return precedences[peek_token.type];
};