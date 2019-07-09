#include "c_libraries/math.hpp"
#include "c_std.hpp"

map<string, env_provider> c_libraries = {
    {"math", &math_env}
};