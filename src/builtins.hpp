#ifndef BUILTIN_H
#define BUILTIN_H

#include "object.hpp"
#include <map>
#include <string>

using namespace std;

extern map<string, Builtin*> builtins;

#endif