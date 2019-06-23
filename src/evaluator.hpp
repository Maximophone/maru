#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "object.hpp"
#include "ast.hpp"

extern Null* NULL_;
extern Boolean* TRUE;
extern Boolean* FALSE;

Object* eval(Node*, Environment* env);
Object* eval_program(Program*, Environment* env);
Object* eval_block_statement(BlockStatement*, Environment* env);
Object* eval_prefix_expression(string, Object*, Environment* env);
Object* eval_bang_operator_expression(Object*, Environment* env);
Object* eval_minus_prefix_operator_expression(Object*, Environment* env);
Object* eval_infix_expression(string, Object*, Object*, Environment* env);
Object* eval_integer_infix_expression(string, Object*, Object*, Environment* env);
Object* eval_string_infix_expression(string, Object*, Object*, Environment* env);
Object* eval_if_expression(IfExpression*, Environment* env);
Object* eval_identifier(Identifier*, Environment* env);
vector<Object*> eval_expressions(vector<Expression*>, Environment* env);
Object* apply_function(Object*, vector<Object*>);
Object* eval_index_expression(Object* left, Object* index);
Object* eval_array_index_expression(Object* array, Object* index);
Object* eval_hash_index_expression(Object*, Object*);
Object* eval_hash_literal(HashLiteral*, Environment*);
Environment* extend_function_env(Function*, vector<Object*>);
Object* unwrap_return_value(Object*);
bool is_truthy(Object*);
Error* new_error(string);
bool is_error(Object*);

#endif