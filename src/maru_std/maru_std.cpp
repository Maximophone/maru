#define STRINGIFY(x) #x

#include "maru_std.hpp"

const string math_str = 
#include "math.mu"
;

map<string, string> maru_libraries = {
    {"math", math_str},
};
