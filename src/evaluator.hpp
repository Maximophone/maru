#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "object.hpp"
#include "ast.hpp"

Object* eval(Node*);
Object* eval_statements(vector<Statement*>);

#endif