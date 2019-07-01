#include "evaluator.hpp"
#include "builtins.hpp"

Null* NULL_ = new Null();
Boolean* TRUE = new Boolean(true);
Boolean* FALSE = new Boolean(false);
const int STACK_OVERFLOW_LIMIT = 2000;
int CURRENT_RECURSION_DEPTH = 0;

Object* eval(Node* node, Environment* env){
    if(node == 0){
        return new_error("INTERNAL: trying to evaluate null pointer node");
    }
    if(env == 0){
        return new_error("INTERNAL: trying to evaluate node with null pointer environment");
    }
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
    if(BreakStatement* stmt = dynamic_cast<BreakStatement*>(node)){
        return new Break();
    }
    if(ContinueStatement* stmt = dynamic_cast<ContinueStatement*>(node)){
        return new Continue();
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
    if(ClassLiteral* cl_lit = dynamic_cast<ClassLiteral*>(node)){
        return eval_class_literal(cl_lit, env);
    }
    if(FunctionLiteral* fn_lit = dynamic_cast<FunctionLiteral*>(node)){
        Function* fn = new Function();
        fn->parameters = fn_lit->parameters;
        fn->body = fn_lit->body;
        fn->env = env;
        return fn;
    }
    if(CallExpression* exp = dynamic_cast<CallExpression*>(node)){
        Object* callable = eval(exp->callable, env);
        if(callable == 0){
            return new_error("INTERNAL: Callable is null pointer");
        }
        if(is_error(callable))
            return callable;
        vector<Object*> args = eval_expressions(exp->arguments, env);
            if(args.size()==1 && is_error(args[0])){
                return args[0];
            }
        if(Function* function = dynamic_cast<Function*>(callable)){
            return apply_function(function, args);
        }
        if(Builtin* builtin = dynamic_cast<Builtin*>(callable)){
            return apply_function(builtin, args);
        }
        if(Class* cl = dynamic_cast<Class*>(callable)){
            ClassInstance* cl_i = new ClassInstance();
            cl_i->attributes = cl->attributes;
            cl_i->env = 0;
            Environment* instance_env = new Environment(cl->env);
            // For all methods in the class, we will create a 
            // copy in the instance, with its own environment
            for(Identifier* attr: cl->attributes){
                bool ok = true;
                Object* val = instance_env->get(attr->value, ok);
                if(!ok)
                    continue;
                if(Function* fn = dynamic_cast<Function*>(val)){
                    Function* new_fn = new Function();
                    new_fn -> parameters = fn->parameters;
                    new_fn -> body = fn->body;
                    if(fn->env == 0){
                        return new_error("INTERNAL: class method internal environment is null pointer");
                    }
                    new_fn -> env = fn->env->copy();
                    if(new_fn == 0){
                        return new_error("INTERNAL: class method copy of internal environment is null pointer");
                    }
                    instance_env->set(attr->value, new_fn);
                }
            }
            cl_i->env = instance_env;
            //instance_env->set("self", cl_i);
            if(cl->constructor!=0){
                apply_method(cl_i, cl->constructor, args);
            }
            return cl_i;
        }
        return new_error("object of type " + callable->type + " cannot be called");
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
    if(AccessExpression* exp = dynamic_cast<AccessExpression*>(node)){
        Object* object = eval(exp->object, env);
        if(is_error(object))
            return object;
        return eval_access_expression(object, exp->attribute);
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
        if(Break* br = dynamic_cast<Break*>(result)){
            return br;
        }
        if(Continue* ct = dynamic_cast<Continue*>(result)){
            return ct;
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
    String* left_str = dynamic_cast<String*>(left);
    String* right_str = dynamic_cast<String*>(right);
    if((left_str == 0) || (right_str == 0)){
        return new_error("not a string (how did you even get that here?)");
    }
    string left_val = left_str->value;
    string right_val = right_str->value;
    if(op=="+"){
        return new String(left_val + right_val);
    };
    if(op=="=="){
        return (left_val==right_val)?TRUE:FALSE;
    };
    if(op=="!="){
        return (left_val!=right_val)?TRUE:FALSE;
    };
    return new_error("unknown operator: " + left->type + op + right->type);
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
        if(is_break(body_value)){
            return NULL_;
        }
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
        if(is_break(body_value)){
            return NULL_;
        }
        condition = eval(exp->condition, env);
        if(is_error(condition))
            return condition;
    }
    return body_value;
};

Object* eval_class_literal(ClassLiteral* cl_lit, Environment* env){
    Class* cl = new Class();
    cl->env = new Environment();
    cl->constructor = 0;
    for(Statement* stmt : cl_lit->body->statements){
        ExpressionStatement* exp_stmt = dynamic_cast<ExpressionStatement*>(stmt);
        if(exp_stmt == 0){
            return new_error("class declaration does not accept statements of this type: " + stmt->to_string());
        }
        Environment* self_env = new Environment(env);
        if(AssignExpression* exp = dynamic_cast<AssignExpression*>(exp_stmt->expression)){
            Identifier* name = dynamic_cast<Identifier*>(exp->name);
            if(name==0)
                return new_error("class attributes error: " + name->to_string());
            cl->attributes.push_back(name);
            Object* value = eval(exp->value, self_env);
            if(is_error(value))
                return value;
            cl->env->set(name->value, value);
        }
        else if (FunctionLiteral* fn_lit = dynamic_cast<FunctionLiteral*>(exp_stmt->expression)){
            Object* fn = eval(fn_lit, self_env);
            if(is_error(fn))
                return fn;
            cl->constructor = (Function*) fn;
        }
        else {
            return new_error("class declaration does not accept expressions of this type: " + exp_stmt->to_string());
        }
    }
    return cl;
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

Object* apply_method(Object* inst, Object* fn, vector<Object*> args){
    if(ClassInstance* instance = dynamic_cast<ClassInstance*>(inst)){
        if(Function* function = dynamic_cast<Function*>(fn)){
            if(args.size()<function->parameters.size()){
                return new_error("not enough arguments, expected " + to_string(function->parameters.size()));
            }
            Environment* extended_env = extend_function_env(function, args);
            CURRENT_RECURSION_DEPTH += 1;
            extended_env->set("self", instance);
            Object* evaluated = eval(function->body, extended_env);
            CURRENT_RECURSION_DEPTH -= 1;
            return unwrap_return_value(evaluated);
        }
        return new_error("Not a function: " + fn->type);
    }
    return new_error("Not an instance: " + inst->type);
};

Object* eval_assign_expression(AssignExpression* exp, Environment* env){
    Object* value = eval(exp->value, env);
    if(is_error(value))
        return value;
    if(Identifier* ident = dynamic_cast<Identifier*>(exp->name)){
        env->set(ident->value, value);
        return value;
    }
    if(AccessExpression* acc_exp = dynamic_cast<AccessExpression*>(exp->name)){
        Object* obj = eval(acc_exp->object, env);
        if(is_error(obj))
            return obj;
        ClassInstance* inst = dynamic_cast<ClassInstance*>(obj);
        if(inst == 0)
            return new_error("can't set attributes on object of type " + obj->type);
        inst->env->set(acc_exp->attribute->value, value);
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

Object* eval_access_expression(Object* left, Identifier* ident){
    Object* value = NULL_;
    bool ok = true;
    if(left->type == INSTANCE_OBJ){
        ClassInstance* instance = (ClassInstance*) left;
        if(instance->env == 0){
            return new_error("INTERNAL: instance environment is null pointer");
        }
        if(ident == 0){
            return new_error("INTERNAL: in access expression, identifier is null pointer");
        }
        value = instance->env->get(ident->value, ok);
        if(ok){
            if(Function* fn = dynamic_cast<Function*>(value)){
                // VERY WRONG WAY TO DO IT
                fn->env->set("self", instance);
                // When I do it this way, I can't have recursive 
                // method calls, because the environment of all 
                // methods is set at the same time (the methods are
                // not duplicated per instance, they are global...)
            }
            return value;
        }
        return NULL_;
    }
    if(left->type == CLASS_OBJ){
        Class* cl = (Class*) left;
        value = cl->env->get(ident->value, ok);
        if(ok)
            return value;
        return NULL_;
    }
    return new_error("can't access attribute on type " + left->type);
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

bool is_break(Object* obj){
    return dynamic_cast<Break*>(obj)!=0;
};

bool is_continue(Object* obj){
    return dynamic_cast<Continue*>(obj)!=0;
};