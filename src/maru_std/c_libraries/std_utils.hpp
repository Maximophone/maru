#ifndef STD_UTILS_H
#define STD_UTILS_H

#include "../../object.hpp"
#include "../../evaluator.hpp"

#include <set>

Error* single_num_arg(vector<Object*>, double&);
Error* not_one_arg(vector<Object*> args);
Error* check_signature(vector<Object*>, vector<set<ObjectType>>);

#endif