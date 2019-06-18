#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "object.hpp"
#include "ast.hpp"

Object* eval(Node*);
Object* eval_statements(vector<Statement*>);
Object* eval_prefix_expression(string, Object*);
Object* eval_bang_operator_expression(Object*);

#endif