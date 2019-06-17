#include <string>
#include "ast.hpp"

using namespace std;

string Program::token_literal(){
    if(statements.size()>0){
        return statements[0]->token_literal();
    } else {
        return "";
    }
};

string Program::to_string(){
    string s = "";
    for(Statement* stmt : statements){
        s += stmt->to_string();
    }
    return s;
}

string Expression::token_literal(){
    return token.literal;
};

string Statement::token_literal(){
    return token.literal;
};

string LetStatement::to_string(){
    string s = token_literal() + " ";
    s += name->to_string() + " = ";
    if(value!=0){
        s += value->to_string();
    }
    s += "; ";
    return s;
};

string ReturnStatement::to_string(){
    string s = token_literal() + " ";
    if(return_value!=0){
        s += return_value->to_string();
    }
    s += "; ";
    return s;
};

string ExpressionStatement::to_string(){
    if(expression!=0)
        return expression->to_string();
    return "";
};

string BlockStatement::to_string(){
    string s = "";
    for(Statement* stmt : statements){
        s += stmt->to_string();
    }
    return s;
};

string Identifier::to_string(){
    return value;
};

string IntegerLiteral::to_string(){
    return std::to_string(value);
};

string BooleanLiteral::to_string(){
    return value?"true":"false";
};

string PrefixExpression::to_string(){
    return "(" + op + right->to_string() + ")";
};

string InfixExpression::to_string(){
    return "(" + left_value->to_string() + op + right_value->to_string() + ")";
};

string IfExpression::to_string(){
    string s = "if(" + condition->to_string() + "){" + consequence->to_string() + "}";
    if(alternative != 0){
        s+= "else{" + alternative->to_string() + "}";
    };
    return s;
};

