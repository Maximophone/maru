#include "evaluator.hpp"
#include "builtins.hpp"

Null* NULL_ = new Null();
Boolean* TRUE = new Boolean(true);
Boolean* FALSE = new Boolean(false);
const int STACK_OVERFLOW_LIMIT = 2000;
int CURRENT_RECURSION_DEPTH = 0;

Object* eval(Node* node, Environment* env){
    // STACK OVERFLOW ERROR
    if(CURRENT_RECURSION_DEPTH>=STACK_OVERFLOW_LIMIT){
        return new_error("stack overflow, recursion depth cannot exceed " + to_string(STACK_OVERFLOW_LIMIT));
    }
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
    if(StringLiteral* lit = dynamic_cast<StringLiteral*>(node)){
        return new String(lit->value);
    }
    if(AssignExpression* exp = dynamic_cast<AssignExpression*>(node)){
        return eval_assign_expression(exp, env);
    }
    if(ArrayLiteral* lit = dynamic_cast<ArrayLiteral*>(node)){
        vector<Object*> elements = eval_expressions(lit->elements, env);
        if(elements.size() == 1 && is_error(elements[0])){
            return elements[0];
        }
        Array* arr = new Array();
        arr->elements = elements;
        return arr;
    }
    if(HashLiteral* lit = dynamic_cast<HashLiteral*>(node)){
        return eval_hash_literal(lit, env);
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
    if(ForExpression* exp = dynamic_cast<ForExpression*>(node)){
        return eval_for_expression(exp, env);
    }
    if(WhileExpression* exp = dynamic_cast<WhileExpression*>(node)){
        return eval_while_expression(exp, env);
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
    if(IndexExpression* exp = dynamic_cast<IndexExpression*>(node)){
        Object* left = eval(exp->left, env);
        if(is_error(left))
            return left;
        Object* index = eval(exp->index, env);
        if(is_error(index))
            return index;
        return eval_index_expression(left, index);
    }
    return 0;
}

Object* eval_program(Program* program, Environment* env){
    Object* result = 0;
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
    Object* result = 0;
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
    if(dynamic_cast<String*>(left) && dynamic_cast<String*>(right)){
        return eval_string_infix_expression(op, left, right, env);
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

Object* eval_string_infix_expression(string op, Object* left, Object* right, Environment* env){
    if(op!="+")
        return new_error("unknown operator: " + left->type + op + right->type);
    String* left_str = dynamic_cast<String*>(left);
    String* right_str = dynamic_cast<String*>(right);
    if((left_str == 0) || (right_str == 0)){
        return new_error("not a string (how did you even get that here?)");
    }
    string left_val = left_str->value;
    string right_val = right_str->value;
    return new String(left_val + right_val);
}

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

Object* eval_for_expression(ForExpression* exp, Environment* env){
    Object* iterated_obj = eval(exp->iterated, env);
    if(is_error(iterated_obj))
        return iterated_obj;
    Array* iterated = dynamic_cast<Array*>(iterated_obj);
    if(iterated == 0)
        return new_error("can only iterate over ARRAY objects, got " + iterated_obj->type);
    Object* body_value = NULL_;
    for(Object* element : iterated->elements){
        env->set(exp->iterator->value, element);
        body_value = eval_block_statement(exp->body, env);
        if(is_error(body_value) || is_return(body_value))
            return body_value;
    }
    return body_value;
};

Object* eval_while_expression(WhileExpression* exp, Environment* env){
    Object* condition = eval(exp->condition, env);
    if(is_error(condition))
        return condition;
    Object* body_value = NULL_;
    while(is_truthy(condition)){
        body_value = eval_block_statement(exp->body, env);
        if(is_error(body_value) || is_return(body_value))
            return body_value;
        condition = eval(exp->condition, env);
        if(is_error(condition))
            return condition;
    }
    return body_value;
};

Object* eval_identifier(Identifier* ident, Environment* env){
    bool ok = true;
    Object* val = env->get(ident->value, ok);
    if(ok){
        return val;
    }
    if(Builtin* builtin = builtins[ident->value]){
        return builtin;
    }
    return new_error("identifier not found: " + ident->value);
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
    if(Function* function = dynamic_cast<Function*>(fn)){
        if(args.size()<function->parameters.size()){
            return new_error("not enough arguments, expected " + to_string(function->parameters.size()));
        }
        Environment* extended_env = extend_function_env(function, args);
        CURRENT_RECURSION_DEPTH += 1;
        Object* evaluated = eval(function->body, extended_env);
        CURRENT_RECURSION_DEPTH -= 1;
        return unwrap_return_value(evaluated);
    }
    if(Builtin* builtin = dynamic_cast<Builtin*>(fn)){
        return builtin->fn(args);
    }
    return new_error("Not a function: " + fn->type);
    
};

Object* eval_assign_expression(AssignExpression* exp, Environment* env){
    Object* value = eval(exp->value, env);
    if(is_error(value))
        return value;
    if(Identifier* ident = dynamic_cast<Identifier*>(exp->name)){
        env->set(ident->value, value);
        return value;
    }
    return new_error("can't assign to expression of this type");
};

Object* eval_index_expression(Object* left, Object* index){
    if(left->type == ARRAY_OBJ && index->type == INTEGER_OBJ){
        return eval_array_index_expression(left, index);
    }
    if(left->type == HASH_OBJ){
        return eval_hash_index_expression(left, index);
    }
    return new_error("index operator not supported: "+left->type+"["+index->type+"]");
};

Object* eval_array_index_expression(Object* left, Object* index){
    Array* arr = (Array*) left;
    int idx = ((Integer*)index)->value;
    int max = arr->elements.size() - 1;
    if(idx < 0 || idx > max){
        return NULL_;
    }
    return arr->elements[idx];
};

Object* eval_hash_index_expression(Object* left, Object* index){
    Hash* hash = (Hash*) left;
    Hashable* index_hashable = dynamic_cast<Hashable*>(index);
    if(index_hashable == 0)
        return new_error("unusable as hash key: " + index->type);
    HashKey key = hash_key(index_hashable);
    if(hash->pairs.find(key) == hash->pairs.end())
        return NULL_;
    HashPair pair = hash->pairs[hash_key(index_hashable)];
    return pair.value;
};

Object* eval_hash_literal(HashLiteral* hash_lit, Environment* env){
    map<HashKey, HashPair> pairs;
    for(pair<Expression*, Expression*> p : hash_lit->pairs){
        Object* key = eval(p.first, env);
        if(is_error(key))
            return key;
        Hashable* hk = dynamic_cast<Hashable*>(key);
        if(hk == 0)
            return new_error("unusable as hash key: " + key->type);
        Object* value = eval(p.second, env);
        if(is_error(value))
            return value;
        HashKey hashed = hash_key(hk);
        pairs[hashed] = HashPair{key, value};
    }
    Hash* hash = new Hash();
    hash->pairs = pairs;
    return hash;
};

Environment* extend_function_env(Function* function, vector<Object*> args){
    Environment* env = new Environment(function->env);

    for(int i = 0; i<function->parameters.size(); i++){
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

bool is_return(Object* obj){
    return dynamic_cast<ReturnValue*>(obj)!=0;
};