#include "evaluator.hpp"

Null* NULL_ = new Null();
Boolean* TRUE = new Boolean(true);
Boolean* FALSE = new Boolean(false);

Object* eval(Node* node, Environment* env){
    // STATEMENTS
    if(Program* program = dynamic_cast<Program*>(node)){
        return eval_program(program, env);
    }
    if(ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(node)){
        return eval(stmt->expression, env);
    }
    if(BlockStatement* stmt = dynamic_cast<BlockStatement*>(node)){
        return eval_block_statement(stmt, env);
    }
    if(ReturnStatement* stmt = dynamic_cast<ReturnStatement*>(node)){
        Object* val = eval(stmt->return_value, env);
        if(is_error(val))
            return val;
        return new ReturnValue(val);
    }
    if(LetStatement* stmt = dynamic_cast<LetStatement*>(node)){
        Object* val = eval(stmt->value, env);
        if(is_error(val))
            return val;
        env->set(stmt->name->value, val);
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
        Object* right = eval(exp->right, env);
        if(is_error(right))
            return right;
        return eval_prefix_expression(exp->op, right, env);
    }
    if(InfixExpression* exp = dynamic_cast<InfixExpression*>(node)){
        Object* left = eval(exp->left_value, env);
        if(is_error(left))
            return left;
        Object* right = eval(exp->right_value, env);
        if(is_error(right))
            return right;
        return eval_infix_expression(exp->op, left, right, env);
    }
    if(IfExpression* exp = dynamic_cast<IfExpression*>(node)){
        return eval_if_expression(exp, env);
    }
    if(Identifier* ident = dynamic_cast<Identifier*>(node)){
        return eval_identifier(ident, env);
    }
    if(FunctionLiteral* fn_lit = dynamic_cast<FunctionLiteral*>(node)){
        Function* fn = new Function();
        fn->parameters = fn_lit->parameters;
        fn->body = fn_lit->body;
        fn->env = env;
        return fn;
    }
    if(CallExpression* exp = dynamic_cast<CallExpression*>(node)){
        Object* function = eval(exp->function, env);
        if(is_error(function))
            return function;
        vector<Object*> args = eval_expressions(exp->arguments, env);
        if(args.size()==1 && is_error(args[0])){
            return args[0];
        }
        return apply_function(function, args);
    }
    return 0;
}

Object* eval_program(Program* program, Environment* env){
    Object* result;
    for(Statement* stmt : program->statements){
        result = eval(stmt, env);
        if(ReturnValue* ret_val = dynamic_cast<ReturnValue*>(result)){
            return ret_val->value;
        }
        if(Error* err = dynamic_cast<Error*>(result)){
            return err;
        }
    }
    return result;
};

Object* eval_block_statement(BlockStatement* block, Environment* env){
    Object* result;
    for(Statement* stmt : block->statements){
        result = eval(stmt, env);
        if(ReturnValue* ret_val = dynamic_cast<ReturnValue*>(result)){
            return ret_val;
        }
        if(Error* err = dynamic_cast<Error*>(result)){
            return err;
        }
    }
    return result;
};

Object* eval_prefix_expression(string op, Object* right, Environment* env){
    if(op=="!"){
        return eval_bang_operator_expression(right, env);
    };
    if(op=="-"){
        return eval_minus_prefix_operator_expression(right, env);
    };
    return new_error("unknown operator: " + op + right->type);
};

Object* eval_bang_operator_expression(Object* right, Environment* env){
    return is_truthy(right)?FALSE:TRUE;
};

Object* eval_minus_prefix_operator_expression(Object* right, Environment* env){
    
    if(Integer* int_obj = dynamic_cast<Integer*>(right))
        return new Integer(-int_obj->value);
    return new_error("unknown operator: -" + right->type);
};

Object* eval_infix_expression(string op, Object* left, Object* right, Environment* env){
    if(dynamic_cast<Integer*>(left) && dynamic_cast<Integer*>(right)){
        return eval_integer_infix_expression(op, left, right, env);
    }
    if(op == "==")
        return (left==right)?TRUE:FALSE;
    if(op == "!=")
        return (left!=right)?TRUE:FALSE;
    if(left->type != right->type){
        return new_error("type mismatch: " + left->type + op + right->type);
    }
    return new_error("unknown operator: " + left->type + op + right->type);
};

Object* eval_integer_infix_expression(string op, Object* left, Object* right, Environment* env){
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
    return new_error("unknown operator: " + left->type + op + right->type);
};

Object* eval_if_expression(IfExpression* exp, Environment* env){
    Object* condition = eval(exp->condition, env);
    if(is_error(condition))
        return condition;
    if(is_truthy(condition)){
        return eval(exp->consequence, env);
    } else if (exp->alternative!=0){
        return eval(exp->alternative, env);
    } else {
        return NULL_;
    }
};

Object* eval_identifier(Identifier* ident, Environment* env){
    bool ok = true;
    Object* val = env->get(ident->value, ok);
    if(!ok){
        return new_error("identifier not found: " + ident->value);
    }
    return val;
};

vector<Object*> eval_expressions(vector<Expression*> exps, Environment* env){
    vector<Object*> result;
    for(Expression* exp : exps){
        Object* evaluated = eval(exp, env);
        if(is_error(evaluated))
            return {evaluated};
        result.push_back(evaluated);
    }
    return result;
};

Object* apply_function(Object* fn, vector<Object*> args){
    Function* function = dynamic_cast<Function*>(fn);
    if(function == 0)
        return new_error("Not a function: " + fn->type);
    Environment* extended_env = extend_function_env(function, args);
    Object* evaluated = eval(function->body, extended_env);
    return unwrap_return_value(evaluated);
};

Environment* extend_function_env(Function* function, vector<Object*> args){
    Environment* env = new Environment(function->env);

    for(int i = 0; i<args.size(); i++){
        env->set(function->parameters[i]->value, args[i]);
    }

    return env;
};

Object* unwrap_return_value(Object* obj){
    if(ReturnValue* ret_val = dynamic_cast<ReturnValue*>(obj))
        return ret_val->value;
    return obj;
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

Error* new_error(string message){
    return new Error(message);
};

bool is_error(Object* obj){
    if(obj != 0){
        return obj->type == ERROR_OBJ;
    }
    return false;
};