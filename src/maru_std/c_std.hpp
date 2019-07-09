#ifndef C_STD
#define C_STD

#include "../object.hpp"

typedef Environment* (*env_provider)();

extern map<string, env_provider> c_libraries;

#endif