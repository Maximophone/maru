#include "evaluator.hpp"

Object* eval(Node* node){
    // STATEMENTS
    if(Program* program = dynamic_cast<Program*>(node)){
        return eval_statements(program->statements);
    }
    if(ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(node)){
        return eval(stmt->expression);
    }

    // EXPRESSIONS
    if(IntegerLiteral* lit = dynamic_cast<IntegerLiteral*>(node)){
        Integer* i = new Integer();
        i->value = lit->value;
        return i;
    }
    return 0;
}

Object* eval_statements(vector<Statement*> statements){
    Object* result;
    for(Statement* stmt : statements){
        result = eval(stmt);
    }
    return result;
};