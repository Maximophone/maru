#include "catch.hpp"
#include "../evaluator.hpp"
#include "../parser.hpp"
#include "test_utils.hpp"
#include <string>

using namespace std;

Object* test_eval(string input){
    Lexer* l = new Lexer(input);
    Parser* p = new Parser(l);
    Program* program = p->parse_program();

    check_parser_errors(p);
    REQUIRE(program != 0);

    Environment* env = new Environment();
    return eval(dynamic_cast<Node*>(program), env);
};

TEST_CASE("test eval integer expression"){
    struct test {
        string input;
        int expected;
    };
    vector<test> tests = {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
        {"5 + 5 + 5 + 5 - 10", 10},
        {"2 * 2 * 2 * 2 * 2", 32},
        {"-50 + 100 + -50", 0},
        {"5 * 2 + 10", 20},
        {"5 + 2 * 10", 25},
        {"20 + 2 * -10", 0},
        {"50 / 2 * 2 + 10", 60},
        {"2 * (5 + 10)", 30},
        {"3 * 3 * 3 + 10", 37},
        {"3 * (3 * 3) + 10", 37},
        {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
    };

    for(test t : tests){
        Object* evaluated = test_eval(t.input);
        test_integer_object(evaluated, t.expected);
    }
};

TEST_CASE("test eval boolean expression"){
    struct test {
        string input;
        bool expected;
    };
    vector<test> tests = {
        {"true", true},
        {"false", false},
        {"1 <2", true},
        {"1 >2", false},
        {"1 <1", false},
        {"1>1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 == 2", false},
        {"1 != 2", true},
        {"true == true", true},
        {"false == false", true},
        {"true == false", false},
        {"true != false", true},
        {"false != true", true},
        {"(1 <2) == true", true},
        {"(1 <2) == false", false},
        {"(1 >2) == true", false},
        {"(1 >2) == false", true},
    };

    for(test t : tests){
        Object* evaluated = test_eval(t.input);
        test_boolean_object(evaluated, t.expected);
    }
};

TEST_CASE("test bang operator"){
    struct test {
        string input;
        bool expected;
    };
    vector<test> tests = {
        {"!true", false},
        {"!false", true},
        {"!5", false},
        {"!!true", true},
        {"!!false", false},
        {"!!5", true},
    };

    for(test t : tests){
        Object* evaluated = test_eval(t.input);
        test_boolean_object(evaluated, t.expected);
    }
};

TEST_CASE("test if else expressions"){
    struct test{
        string input;
        Var expected;
    };
    vector<test> tests = {
        {"if(true) {10}", Var(10)},
        {"if(false){8}", Var()},
        {"if (1) {10}", Var(10)},
        {"if (1 < 2) { 10 }", Var(10)},
        {"if (1 > 2) { 10 }", Var()},
        {"if (1 > 2) { 10 } else { 20 }", Var(20)},
        {"if (1 < 2) { 10 } else { 20 }", Var(10)},
        {"if (\"\" == \"\") {1} else {0}", Var(1)},
        {"let x = \"blah\"; if (\"blah\" == x){1}", Var(1)},
        {"let x = \"\"; if(x != \"test\"){1;}", Var(1)},
    };

    for (test t : tests){
        INFO("Expression: " + t.input);
        Object* evaluated = test_eval(t.input);
        test_var_object(evaluated, t.expected);
    }
};

TEST_CASE("test eval for loop"){
    struct test{
        string input;
        Var expected;
    };
    vector<test> tests = {
        {"for(x in range(5)){}; x;", Var(4)},
        {"for(x in range(5)){x+1};", Var(5)},
        {"let i = 0; for(x in range(5)){let i = i + x}; i;", Var(10)},
        {"for(x in 2){}", Var("can only iterate over ARRAY objects, got INTEGER"s)},
    };

    for(test t : tests){
        INFO("Input: " + t.input);
        test_var_object(test_eval(t.input), t.expected);
    }
};

TEST_CASE("test eval while loop"){
    struct test{
        string input;
        Var expected;
    };
    vector<test> tests = {
        {"x=0; while(x<1){x=x+1;}; x;", Var(1)},
        {"x=0; while(x<10){x=x+1;}", Var(10)},
    };

    for(test t : tests){
        INFO("Input: " + t.input);
        test_var_object(test_eval(t.input), t.expected);
    }
};

TEST_CASE("test return statements"){
    struct test{
        string input;
        int expected;
    };
    vector<test> tests = {
        {"return 10;", 10},
        {"return 10; 9;", 10},
        {"return 2 * 5; 9;", 10},
        {"9; return 2 * 5; 9;", 10},
        {"if (10 > 1) {"
            "if (10 > 1) {"
                "return 10;"
            "}"
            "return 1;"
        "}", 10},
    };

    for(test t : tests){
        INFO("Expression: " + t.input);
        Object* evaluated = test_eval(t.input);
        test_integer_object(evaluated, t.expected);
    }
};

TEST_CASE("test error handling"){
    struct test {
        string input;
        string expected_message;
    };
    vector<test> tests = {
        {
            "5 + true;",
            "type mismatch: INTEGER+BOOLEAN",
        },
        {
            "5 + true; 5;",
            "type mismatch: INTEGER+BOOLEAN",
        },
        {
            "-true",
            "unknown operator: -BOOLEAN",
        },
        {
            "true + false;",
            "unknown operator: BOOLEAN+BOOLEAN",
        },
        {
            "5; true + false; 5",
            "unknown operator: BOOLEAN+BOOLEAN",
        },
        {
            "if (10 > 1) { true + false; }",
            "unknown operator: BOOLEAN+BOOLEAN",
        },
        {
"if (10 > 1) {"
  "if (10 > 1) {"
    "return true + false;"
  "}"
  "return 1;"
"}",
            "unknown operator: BOOLEAN+BOOLEAN",
        },
        {"foobar", "identifier not found: foobar"},
        {"\"Hello\" - \"World\"", "unknown operator: STRING-STRING"},
        {"let x = fn(a){}; x()", "not enough arguments, expected 1"},
        {"print(1)", "arguments to 'print' must be STRING, got INTEGER"},
        {"let a = fn(){a();}; a();", "stack overflow, recursion depth cannot exceed " + to_string(STACK_OVERFLOW_LIMIT)},
        {
            "{5:2}[fn(x){x}]",
            "unusable as hash key: FUNCTION",
        },
        {"X = class{fn(){X();}};X();", "stack overflow, recursion depth cannot exceed " + to_string(STACK_OVERFLOW_LIMIT)},
    };

    for(test t : tests){
        INFO("Input: " + t.input);
        Object* evaluated = test_eval(t.input);
        test_error_object(evaluated, t.expected_message);
    }
};

TEST_CASE("test evaluating let statements"){
    struct test {
        string input;
        int expected;
    };
    vector<test> tests = {
        {"let a = 5; a;", 5},
        {"let a = 5 * 5; a;", 25},
        {"let a = 5; let b = a; b;", 5},
        {"let a = 5; let b = a; let c = a + b + 5; c;", 15},
    };
    for(test t : tests){
        test_integer_object(test_eval(t.input), t.expected);
    };
};

TEST_CASE("test function object"){
    string input = "fn(x){x+2;};";

    Object* evaluated = test_eval(input);
    Function* fn = dynamic_cast<Function*>(evaluated);
    REQUIRE(fn != 0);

    REQUIRE(fn->parameters.size() == 1);
    CHECK(fn->parameters[0]->to_string()=="x");

    string expected_body = "(x+2)";
    CHECK(fn->body->to_string() == expected_body);
};

TEST_CASE("test function application"){
    struct test {
        string input;
        int expected;
    };
    vector<test> tests = {
        {"let identity = fn(x) { x; }; identity(5);", 5},
        {"let identity = fn(x) { return x; }; identity(5);", 5},
        {"let double = fn(x) { x * 2; }; double(5);", 10},
        {"let add = fn(x, y) { x + y; }; add(5, 5);", 10},
        {"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
        {"fn(x) { x; }(5)", 5},
    };

    for(test t : tests){
        test_integer_object(test_eval(t.input), t.expected);
    }
};

TEST_CASE("test closures"){
    string input = "let new_adder = fn(x){"
    "fn(y){x + y};"
    "};"
    "let add_two = new_adder(2);"
    "add_two(2);";

    test_integer_object(test_eval(input), 4);
};

TEST_CASE("test string literal"){
    string input = "\"Hello World!\"";
    
    Object* evaluated = test_eval(input);

    test_string_object(evaluated, "Hello World!");
};

TEST_CASE("test string concatenation"){
    string input = "\"Hello\" + \" \" + \"World!\"";

    Object* evaluated = test_eval(input);
    
    test_string_object(evaluated, "Hello World!");
};

TEST_CASE("test builtin functions"){
    struct test {
        string input;
        Var expected;
    };
    vector<test> tests = {
        {"len(\"\")", Var(0)},
        {"len(\"four\")", Var(4)},
        {"len(\"hello world\")", Var(11)},
        {"len(1)", Var("argument to 'len' not supported, got INTEGER"s)},
        {"len(\"one\", \"two\")", Var("wrong number of arguments. got=2, want=1"s)},
        {"len()", Var("wrong number of arguments. got=0, want=1"s)},
        {"print(\"test\")", Var()},
        {"len([])", Var(0)},
        {"len([1,2,3])", Var(3)},
        {"let x = []; append(x, 3); x[0];", Var(3)},
        {"append([]);", Var("wrong number of arguments. got=1, wanted>1"s)},
        {"append();", Var("wrong number of arguments. got=0, wanted>1"s)},
        {"append([1,2], 3, 4, 5)[3]", Var(4)},
        {"let x = range(5); x[3];", Var(3)},
        {"let x = range(1,5); x[3]", Var(4)},
        {"len(range(5));", Var(5)},
        {"append(range(5), 8)[5]", Var(8)},
        {"range();", Var("wrong number of arguments. range takes 1 or 2 but got=0"s)},
    };

    for(test t : tests){
        INFO("testing input: " + t.input);
        Object* evaluated = test_eval(t.input);
        test_var_object(evaluated, t.expected);
    }
};

TEST_CASE("test array literals"){
    string input = "[1, 2*2, 3+3]";

    Object* evaluated = test_eval(input);

    Array* arr = dynamic_cast<Array*>(evaluated);
    REQUIRE(arr != 0);
    REQUIRE(arr->elements.size() == 3);
    test_integer_object(arr->elements[0], 1);
    test_integer_object(arr->elements[1], 4);
    test_integer_object(arr->elements[2], 6);

};

TEST_CASE("test array index expressions"){
    struct test {
        string input;
        Var expected;
    };
    vector<test> tests = {
        {
            "[1, 2, 3][0]",
            Var(1),
        },
        {
            "[1, 2, 3][1]",
            Var(2),
        },
        {
            "[1, 2, 3][2]",
            Var(3),
        },
        {
            "let i = 0; [1][i];",
            Var(1),
        },
        {
            "[1, 2, 3][1 + 1];",
            Var(3),
        },
        {
            "let myArray = [1, 2, 3]; myArray[2];",
            Var(3),
        },
        {
            "let myArray = [1, 2, 3]; myArray[0] + myArray[1] + myArray[2];",
            Var(6),
        },
        {
            "let myArray = [1, 2, 3]; let i = myArray[0]; myArray[i]",
            Var(2),
        },
        {
            "[1, 2, 3][3]",
            Var(),
        },
        {
            "[1, 2, 3][-1]",
            Var(),
        },
    };

    for(test t: tests){
        Object* evaluated = test_eval(t.input);
        test_var_object(evaluated, t.expected);
    }
};

TEST_CASE("test hash literals"){
    string input = "let two = \"two\""
    "{"
    "\"one\": 10 - 9,"
    "two: 1 + 1,"
    "\"thr\" + \"ee\": 6/2,"
    "4: 4,"
    "true: 5,"
    "false: 6"
    "}";

    Object* evaluated = test_eval(input);
    Hash* hash = dynamic_cast<Hash*>(evaluated);
    REQUIRE(hash != 0);

    map<HashKey, int> expected = {
        {hash_key(new String("one")), 1},
        {hash_key(new String("two")), 2},
        {hash_key(new String("three")), 3},
        {hash_key(new Integer(4)), 4},
        {hash_key(TRUE), 5},
        {hash_key(FALSE), 6},
    };

    REQUIRE(hash->pairs.size() == expected.size());

    for(pair<HashKey, int> p : expected){
        HashKey key = p.first;
        int value = p.second;
        HashPair hash_pair = hash->pairs[key];
        test_integer_object(hash_pair.value, value);
    };
};

TEST_CASE("test hash index expressions"){
    struct test {
        string input;
        Var expected;
    };
    vector<test> tests = {
        {
            "{\"foo\": 5}[\"foo\"]", 
            Var(5)
        },
        {
            "{4: 5}[3]",
            Var()
        },
        {
            "let key = \"foo\"; {\"foo\": 5}[key]",
            Var(5)
        },
        {
            "{}[true]",
            Var()
        },
        {
            "{5: 1}[5]",
            Var(1)
        },
        {
            "{false: 1}[false]",
            Var(1)
        },
    };

    for(test t : tests){
        INFO("Input: " + t.input);
        Object* evaluated = test_eval(t.input);
        test_var_object(evaluated, t.expected);
    }
};

TEST_CASE("test assign expressions"){
    struct test {
        string input;
        Var expected;
    };
    vector<test> tests = {
        {"x=2;", Var(2)},
        {"x=3*2; x;", Var(6)},
        {"(x=3)*2;", Var(6)},
        {"(x=3)*2; x;", Var(3)},
        {"x=3*(y=2);y;", Var(2)},
        {"3=5", Var("can't assign to expression of this type"s)}
    };

    for(test t : tests){
        INFO("Input: " + t.input);
        Object* evaluated = test_eval(t.input);
        test_var_object(evaluated, t.expected);
    }
};

TEST_CASE("test create class"){
    string input = ""
    "parent_class = class{x=1;};"
    "my_class = class(parent_class){"
    "a=2;"
    "b=fn(){self.a};"
    "fn(){a=3};"
    "}; my_class;";

    Object* evaluated = test_eval(input);
    Class* class_object = req_cast<Class*>(evaluated);

    REQUIRE(class_object->env != 0);
    REQUIRE(class_object->attributes.size() == 3);

    test_identifier(class_object->attributes[0], "a");
    test_identifier(class_object->attributes[1], "b");
    test_identifier(class_object->attributes[2], "x");

    REQUIRE(class_object->parent != 0);
    REQUIRE(class_object->parent->attributes.size() == 1);

    test_identifier(class_object->parent->attributes[0], "x");
};

TEST_CASE("test create instance"){
    string input = ""
    "cl = class{"
    "a=1;"
    "fn(){self.a=2}"
    "};"
    "inst = cl();";

    Object* evaluated = test_eval(input);
    ClassInstance* cl_i = req_cast<ClassInstance*>(evaluated);

    REQUIRE(cl_i->env != 0);
    REQUIRE(cl_i->attributes.size() == 1);

    test_identifier(cl_i->attributes[0], "a");
};

TEST_CASE("test inheritance"){
    string input = ""
    "X = class{"
    "   a=1;"
    "   fn(x){self.a=x}"
    "};"
    "Y = class(X){"
    "   b=3;"
    "};"
    "y = Y(2);"
    "y;";

    Object* evaluated = test_eval(input);
    ClassInstance* cl_i = req_cast<ClassInstance*>(evaluated);

    REQUIRE(cl_i->env != 0);
    REQUIRE(cl_i->attributes.size() == 2);

    test_identifier(cl_i->attributes[0], "b");
    test_identifier(cl_i->attributes[1], "a");

    bool ok = true;
    test_integer_object(cl_i->env->get("a", ok), 2);
    test_integer_object(cl_i->env->get("b", ok), 3);
};

TEST_CASE("test inheritance 2"){
    string input = ""
    "X = class{"
    "   a=\"\";"
    "   fn(x){self.a=0};"
    "};"
    "Y = class(X){"
    "   b=3;"
    "   fn(x){self.a=x};"
    "};"
    "y = Y(2);"
    "y;";

    Object* evaluated = test_eval(input);
    ClassInstance* cl_i = req_cast<ClassInstance*>(evaluated);

    REQUIRE(cl_i->env != 0);
    REQUIRE(cl_i->attributes.size() == 2);

    test_identifier(cl_i->attributes[0], "b");
    test_identifier(cl_i->attributes[1], "a");

    bool ok = true;
    test_integer_object(cl_i->env->get("a", ok), 2);
    test_integer_object(cl_i->env->get("b", ok), 3);
};

TEST_CASE("test access object attribute"){
    string input = ""
    "cl = class{a=1;};"
    "inst = cl();"
    "inst.a;";

    Object* evaluated = test_eval(input);
    test_integer_object(evaluated, 1);
};

TEST_CASE("test set object attribute"){
    string input = ""
    "cl = class{a=0;};"
    "inst = cl();"
    "inst.a = 2;"
    "inst.a;";

    Object* evaluated = test_eval(input);
    test_integer_object(evaluated, 2);
};

TEST_CASE("test calling object method"){
    string input = ""
    "cl = class{"
        "a = 2;"
        "m = fn(){self.a;};"
    "};"
    "inst = cl();"
    "inst.m();";

    Object* evaluated = test_eval(input);
    test_integer_object(evaluated, 2);
};

TEST_CASE("test calling non existing method"){
    string input = ""
    "T = class{};"
    "t = T();"
    "t.my_method();";

    Object* evaluated = test_eval(input);
    test_error_object(evaluated, "object of type NULL cannot be called");
};

TEST_CASE("test methods"){
    struct test {
        string input;
        Var expected;
    };
    vector<test> tests = {
        {
            "T = class{"
            "rec = fn(i){"
            "if(i<1){return 0};"
            "return self.rec(i-1) + 1;"
            "}};"
            "t = T();"
            "t.rec(10);",
            Var(10)
        },
        {
            "T = class{"
                "a = 1;"
                "m = fn(){return self.a;};"
            "};"
            "self = T();"
            "self.a = 5;"
            "t = T();"
            "t.m();",
            Var(1)
        },
        {
            "T = class{"
                "a = 0;"
                "m = fn(t){"
                    "if(t.a==0){return self.a;};"
                    "t2 = T();"
                    "t2.a = 0;"
                    "return t.m(t2);"
                "};"
            "};"
            "t1 = T(); t2 = T(); t2.a = 1;"
            "t1.m(t2);",
            Var(1)
        },
        {
            "T = class{ id = 0;"
            "m = fn(t){"
            "if(t==0){return -1;}"
            "_ = t.m(0);"
            "return self.id;"
            "}};"
            "t1 = T(); t1.id=0;"
            "t2 = T(); t2.id=1;"
            "t2.m(t1);",
            Var(1)
        }
    };

    for(test t : tests){
        INFO("Input: " + t.input);
        Object* evaluated = test_eval(t.input);
        test_var_object(evaluated, t.expected);
    };
};

TEST_CASE("test break and continue from loop"){
    struct test {
        string input;
        Var expected;
    };
    vector<test> tests = {
        {
            "for(i in range(10)){"
            "if(i==5){"
            "break;"
            "return -1;"
            "}"
            "};"
            "i;",
            Var(5)
        },
        {
            "s = 0;"
            "for(i in range(5)){"
            "if(i==3){"
            "continue;"
            "return -1;"
            "}"
            "s = s + i;"
            "};"
            "s;",
            Var(7)
        },
        {
            "i = 0;"
            "while(true){"
            "i = i + 1;"
            "if(i > 10){"
            "break;"
            "}};"
            "i;",
            Var(11)
        },
    };

    for(test t : tests){
        INFO("Input: " + t.input);

        Object* evaluated = test_eval(t.input);
        test_var_object(evaluated, t.expected);
    }
};

TEST_CASE("test evaluating include statement"){
    string input = "include \"src/resources/program.mu\" as x; x;";

    Object* evaluated = test_eval(input);
    NameSpace* nmspc = req_cast<NameSpace*>(evaluated);

    Environment* nmspc_env = nmspc->env;
    bool ok = true;
    test_integer_object(nmspc_env->get("a", ok), 2);
    test_string_object(nmspc_env->get("b", ok), "test");
    test_boolean_object(nmspc_env->get("c", ok), true);
};

TEST_CASE("test accessing namespace attributes"){
    struct test {
        string input;
        Var expected;
    };

    string inclusion_1 = "include \"src/resources/program.mu\" as inc;";
    string inclusion_2 = "include \"src/resources/program.mu\";";

    vector<test> tests = {
        {
            inclusion_1 + "inc.a",
            Var(2)
        },
        {
            inclusion_1 + "inc.c",
            Var(true)
        },
        {
            inclusion_1 + "inc.d();",
            Var(3)
        },
        {
            inclusion_1 + "inc.e();",
            Var(2)
        },
        {
            inclusion_2 + "a",
            Var(2)
        },
        {
            inclusion_2 + "c",
            Var(true)
        },
        {
            inclusion_2 + "d()",
            Var(3)
        },
        {
            inclusion_2 + "e()",
            Var(2)
        },
    };

    for(test t : tests){
        Object* evaluated = test_eval(t.input);
        test_var_object(evaluated, t.expected);
    }
};

TEST_CASE("test operator overloading"){
    string global_input = ""
    "T = class{"
        "a = 0;"
        "__add__ = fn(other){self.a + other.a};"
        "__sub__ = fn(other){self.a - other.a};"
        "__mul__ = fn(other){self.a * other.a};"
        "__div__ = fn(other){self.a / other.a};"
    "};"
    "t1 = T(); t1.a = 5;"
    "t2 = T(); t2.a = 2;"
    ;

    struct test {
        string input;
        Var expected;
    };
    vector<test> tests = {
        {
            "t1 + t2",
            Var(7)
        },
        {
            "t1 - t2",
            Var(3)
        },
        {
            "t1 * t2",
            Var(10)
        },
        {
            "t1/t2",
            Var(2)
        },
    };

    for(test t : tests){
        Object* evaluated = test_eval(global_input + t.input);
        test_var_object(evaluated, t.expected);
    }
};