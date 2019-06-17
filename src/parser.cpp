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
};

Parser::Parser(Lexer *l)
{
    lexer = l;
    next_token();
    next_token();

    prefix_parse_funcs[IDENT] = &Parser::parse_identifier;
    prefix_parse_funcs[INT] = &Parser::parse_integer_literal;
    prefix_parse_funcs[TRU] = &Parser::parse_boolean_literal;
    prefix_parse_funcs[FALS] = &Parser::parse_boolean_literal;
    prefix_parse_funcs[BANG] = &Parser::parse_prefix_expression;
    prefix_parse_funcs[MINUS] = &Parser::parse_prefix_expression;
    prefix_parse_funcs[LPAREN] = &Parser::parse_grouped_expression;
    prefix_parse_funcs[IF] = &Parser::parse_if_expression;
    prefix_parse_funcs[FUNCTION] = &Parser::parse_function_literal;

    infix_parse_funcs[PLUS] = &Parser::parse_infix_expression;
    infix_parse_funcs[MINUS] = &Parser::parse_infix_expression;
    infix_parse_funcs[SLASH] = &Parser::parse_infix_expression;
    infix_parse_funcs[ASTERIX] = &Parser::parse_infix_expression;
    infix_parse_funcs[EQUAL] = &Parser::parse_infix_expression;
    infix_parse_funcs[NOT_EQUAL] = &Parser::parse_infix_expression;
    infix_parse_funcs[LT] = &Parser::parse_infix_expression;
    infix_parse_funcs[GT] = &Parser::parse_infix_expression;
    infix_parse_funcs[LPAREN] = &Parser::parse_call_expression;
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
    // TODO: We're skipping the expressions until we
    // encounter a semicolon
    while (!cur_token_is(SEMICOLON))
    {
        next_token();
    }
    return stmt;
};

ReturnStatement *Parser::parse_return_statement()
{
    ReturnStatement *stmt = new ReturnStatement();
    stmt->token = cur_token;
    next_token();
    // TODO: We're skipping the expressions until we
    // encounter a semicolon
    while (!expect_peek(SEMICOLON))
    {
        next_token();
    }
    return stmt;
}

ExpressionStatement *Parser::parse_expression_statement()
{
    ExpressionStatement *stmt = new ExpressionStatement();
    stmt->token = cur_token;
    stmt->expression = parse_expression(LOWEST);
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
        no_prefix_parse_func_error(cur_token.type);
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

Expression *Parser::parse_boolean_literal()
{
    BooleanLiteral *lit = new BooleanLiteral();
    lit->token = cur_token;
    lit->value = cur_token_is(TRU);
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

Expression* Parser::parse_call_expression(Expression* function){
    CallExpression* exp = new CallExpression();
    exp->token = cur_token;
    exp->function = function;
    bool ok = true;
    exp->arguments = parse_call_arguments(ok);
    if(!ok){
        return 0;
    }
    return exp;
};

vector<Expression*> Parser::parse_call_arguments(bool& ok){
    vector<Expression*> args = {};
    if(peek_token_is(RPAREN)){
        next_token();
        return args;
    }
    next_token();
    args.push_back(parse_expression(LOWEST));
    while(peek_token_is(COMMA)){
        next_token(); next_token();
        args.push_back(parse_expression(LOWEST));
    }
    if(!expect_peek(RPAREN)){
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

void Parser::no_prefix_parse_func_error(TokenType t)
{
    string message = "no prefix parse function for " + t + " found";
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