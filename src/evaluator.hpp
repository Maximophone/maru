#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "object.hpp"
#include "ast.hpp"

extern Null* NULL_;
extern Boolean* TRUE;
extern Boolean* FALSE;

Object* eval(Node*, Environment*);
Object* eval_program(Program*, Environment*);
Object* eval_block_statement(BlockStatement*, Environment*);
Object* eval_prefix_expression(string, Object*, Environment*);
Object* eval_bang_operator_expression(Object*, Environment*);
Object* eval_minus_prefix_operator_expression(Object*, Environment*);
Object* eval_infix_expression(string, Object*, Object*, Environment*);
Object* eval_integer_infix_expression(string, Object*, Object*, Environment*);
Object* eval_string_infix_expression(string, Object*, Object*, Environment*);
Object* eval_if_expression(IfExpression*, Environment*);
Object* eval_for_expression(ForExpression*, Environment*);
Object* eval_while_expression(WhileExpression*, Environment*);
Object* eval_class_literal(ClassLiteral*, Environment*);
Object* eval_identifier(Identifier*, Environment*);
vector<Object*> eval_expressions(vector<Expression*>, Environment*);
Object* apply_function(Object*, vector<Object*>);
Object* apply_method(Object*, Object*, vector<Object*>);
Object* eval_assign_expression(AssignExpression*, Environment*);
Object* eval_index_expression(Object*, Object*);
Object* eval_array_index_expression(Object*, Object*);
Object* eval_hash_index_expression(Object*, Object*);
Object* eval_hash_literal(HashLiteral*, Environment*);
Environment* extend_function_env(Function*, vector<Object*>);
Object* unwrap_return_value(Object*);
bool is_truthy(Object*);
Error* new_error(string);
bool is_error(Object*);
bool is_return(Object*);

#endif