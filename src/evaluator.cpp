#include "evaluator.hpp"

Null* NULL_ = new Null();
Boolean* TRUE = new Boolean(true);
Boolean* FALSE = new Boolean(false);

Object* eval(Node* node){
    // STATEMENTS
    if(Program* program = dynamic_cast<Program*>(node)){
        return eval_program(program);
    }
    if(ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(node)){
        return eval(stmt->expression);
    }
    if(BlockStatement* stmt = dynamic_cast<BlockStatement*>(node)){
        return eval_block_statement(stmt);
    }
    if(ReturnStatement* stmt = dynamic_cast<ReturnStatement*>(node)){
        Object* val = eval(stmt->return_value);
        return new ReturnValue(val);
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
    if(IfExpression* exp = dynamic_cast<IfExpression*>(node)){
        return eval_if_expression(exp);
    }
    return 0;
}

Object* eval_program(Program* program){
    Object* result;
    for(Statement* stmt : program->statements){
        result = eval(stmt);
        if(ReturnValue* ret_val = dynamic_cast<ReturnValue*>(result)){
            return ret_val->value;
        }
    }
    return result;
};

Object* eval_block_statement(BlockStatement* block){
    Object* result;
    for(Statement* stmt : block->statements){
        result = eval(stmt);
        if(ReturnValue* ret_val = dynamic_cast<ReturnValue*>(result)){
            return ret_val;
        }
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
    return is_truthy(right)?FALSE:TRUE;
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
    if(op == "==")
        return (left==right)?TRUE:FALSE;
    if(op == "!=")
        return (left!=right)?TRUE:FALSE;
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

Object* eval_if_expression(IfExpression* exp){
    Object* condition = eval(exp->condition);
    if(is_truthy(condition)){
        return eval(exp->consequence);
    } else if (exp->alternative!=0){
        return eval(exp->alternative);
    } else {
        return NULL_;
    }
};

bool is_truthy(Object* obj){
    if(obj == NULL_)
        return false;
    if(obj == TRUE)
        return true;
    if(obj == FALSE)
        return false;
    return true;
};