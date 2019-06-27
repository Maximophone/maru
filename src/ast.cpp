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

string AssignExpression::to_string(){
    string s = name->to_string() + " = ";
    if(value != 0){
        s += value->to_string();
    }
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

string StringLiteral::to_string(){
    return value;
};

string ArrayLiteral::to_string(){
    string el_string = "";
    for(int i = 0; i<elements.size(); i++){
        el_string += elements[i]->to_string();
        if(i<elements.size()-1)
            el_string += ",";
    }
    return "["+el_string+"]"; 
};

string HashLiteral::to_string(){
    string pairs_string = "";
    map<Expression*, Expression*>::iterator it;
    int i = 0;
    for(it=pairs.begin(); it!=pairs.end(); it++){
        pairs_string += it->first->to_string() + ":" + it->second->to_string();
        if(i<pairs.size()-1){
            pairs_string += ",";
        }
        i++;
    }
    return "{"+pairs_string+"}";
};

string FunctionLiteral::to_string(){
    string param_string = "";
    for(int i = 0; i<parameters.size(); i++){
        param_string += parameters[i]->to_string();
        if(i<parameters.size()-1)
            param_string += ",";
    }
    return "fn(" + param_string + "){" + body->to_string() + "}";
};

string CallExpression::to_string(){
    string args_string = "";
    for(int i = 0; i<arguments.size(); i++){
        args_string += arguments[i]->to_string();
        if(i<arguments.size()-1)
            args_string += ",";
    }
    return callable->to_string() + "(" + args_string + ")";
};

string IndexExpression::to_string(){
    return "(" + left->to_string() + "[" + index->to_string() + "])";
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

string ForExpression::to_string(){
    return "for(" + iterator->to_string() + " in " + iterated->to_string() + "){" + body->to_string() + "}";
};

string WhileExpression::to_string(){
    return "while(" + condition->to_string() + "){" + body->to_string() + "}";
};

string ClassLiteral::to_string(){
    return "class{" + body->to_string() + "}";
};

string AccessExpression::to_string(){
    return "(" + object->to_string() + "." + attribute->to_string() + ")";
};

string BreakStatement::to_string(){
    return "break;";
};

string ContinueStatement::to_string(){
    return "continue;";
};