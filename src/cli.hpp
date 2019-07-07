#include <string>
#include <map>
#include <vector>
#include <set>

using namespace std;

typedef string ArgType;
const ArgType INT_ARG = "INT";
const ArgType FLOAT_ARG = "FLOAT";
const ArgType STRING_ARG = "STRING";

string lstrip(string, char);

class Arguments {
    public:
        map<string, bool> flags;
        map<string, int> int_args;
        map<string, float> float_args;
        map<string, string> string_args;
        set<string> provided_args;
        bool provided(string arg_name){
            return provided_args.find(arg_name) != provided_args.end();
        };
};

struct ArgumentDef {
    ArgType type;
    bool is_flag;
    bool required;
    string default_value;
    string help;
};

class CliParser {
    private:
        string help = "";
        vector<pair<string, ArgumentDef>> positionals;
        map<string, ArgumentDef> arguments_def;
        map<string, string> aliases;
        void set_arg(Arguments&, ArgumentDef, string, string);
    public:
        void add_argument(string long_name, ArgType type, string help);
        void add_optional(string long_name, ArgType type, string default_val, string help);
        void add_positional(string name, ArgType type, string help);
        void add_positional_optional(string name, ArgType type, string default_val, string help);
        void add_flag(string name, string help);
        void add_alias(string alias, string name);
        Arguments parse(int, char*[]);
        CliParser(){};
        CliParser(string);
};