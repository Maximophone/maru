#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "object.hpp"
#include "ast.hpp"

extern Null* NULL_;
extern Boolean* TRUE;
extern Boolean* FALSE;

Object* eval(Node*);
Object* eval_program(Program*);
Object* eval_block_statement(BlockStatement*);
Object* eval_prefix_expression(string, Object*);
Object* eval_bang_operator_expression(Object*);
Object* eval_minus_prefix_operator_expression(Object*);
Object* eval_infix_expression(string, Object*, Object*);
Object* eval_integer_infix_expression(string, Object*, Object*);
Object* eval_if_expression(IfExpression*);
bool is_truthy(Object*);

#endif