#include "includer.hpp"
#include "evaluator.hpp"
#include "parser.hpp"

#include <fstream>
#include <streambuf>

string FileReader::read(string path){
    std::ifstream t(path);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    return str;
};

bool FileReader::file_exists(string path){
    std::ifstream t(path);
    return t.good();
};

string MockReader::read(string path){
    return mock_file;
};

Includer::Includer(Reader* reader){
    this->reader = reader;
};

incl_result Includer::include(string path){
    if(included.find(path) == included.end()){
        // Can't find key
        if(!reader->file_exists(path)){
            return incl_result{0, {"File does not exist: " + path}, 0};
        }
        string code_string = reader->read(path);

        Lexer* l = new Lexer(code_string);
        Parser* p = new Parser(l);
        Program* program = p->parse_program();

        if(p->errors.size()>0){
            return incl_result{0, p->errors, 0};
        }

        Environment* env = new Environment();
        Object* obj = eval(program, env);

        if(is_error(obj)){
            return incl_result{0, {}, (Error*) obj};
        }

        included[path] = env;
        return incl_result{env, {}, 0};
    } else {
        return incl_result{included[path], {}, 0};
    }
};