#define STRINGIFY(x) #x

#include "maru_std.hpp"

const string vector_str = 
#include "maru_libraries/vector.mu"
;

map<string, string> maru_libraries = {
    {"vector", vector_str},
};
