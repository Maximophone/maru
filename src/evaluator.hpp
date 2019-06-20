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
Object* eval_if_expression(IfExpression*, Environment* env);
Object* eval_identifier(Identifier*, Environment* env);
bool is_truthy(Object*);
Error* new_error(string);
bool is_error(Object*);

#endif