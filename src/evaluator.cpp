#include "evaluator.hpp"

Null* NULL_ = new Null();
Boolean* TRUE = new Boolean(true);
Boolean* FALSE = new Boolean(false);

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
    if(BooleanLiteral* lit = dynamic_cast<BooleanLiteral*>(node)){
        return lit->value?TRUE:FALSE;
    }
    if(PrefixExpression* exp = dynamic_cast<PrefixExpression*>(node)){
        Object* right = eval(exp->right);
        return eval_prefix_expression(exp->op, right);
    }
    if(InfixExpression* exp = dynamic_cast<InfixExpression*>(node)){
        Object* left = eval(exp->left_value);
        Object* right = eval(exp->right_value);
        return eval_infix_expression(exp->op, left, right);
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

Object* eval_prefix_expression(string op, Object* right){
    if(op=="!"){
        return eval_bang_operator_expression(right);
    };
    if(op=="-"){
        return eval_minus_prefix_operator_expression(right);
    };
    return NULL_;
};

Object* eval_bang_operator_expression(Object* right){
    if(right==TRUE){
        return FALSE;
    }
    if(right==FALSE){
        return TRUE;
    }
    if(right==NULL_){
        return TRUE;
    }
    return FALSE;
};

Object* eval_minus_prefix_operator_expression(Object* right){
    
    if(Integer* int_obj = dynamic_cast<Integer*>(right))
        return new Integer(-int_obj->value);
    return NULL_;
};

Object* eval_infix_expression(string op, Object* left, Object* right){
    if(dynamic_cast<Integer*>(left) && dynamic_cast<Integer*>(right)){
        return eval_integer_infix_expression(op, left, right);
    }
    return NULL_;
};

Object* eval_integer_infix_expression(string op, Object* left, Object* right){
    Integer* l = (Integer*) left;
    Integer* r = (Integer*) right;
    if(op=="+")
        return new Integer(l->value + r->value);
    if(op=="-")
        return new Integer(l->value - r->value);
    if(op=="*")
        return new Integer(l->value * r->value);
    if(op=="/")
        return new Integer(l->value / r->value);
    if(op=="<")
        return (l->value < r->value)?TRUE:FALSE;
    if(op==">")
        return (l->value > r->value)?TRUE:FALSE;
    if(op=="==")
        return (l->value == r->value)?TRUE:FALSE;
    if(op=="!=")
        return (l->value != r->value)?TRUE:FALSE;
    return NULL_;
};