#include "catch.hpp"
#include "../includer.hpp"
#include "test_utils.hpp"

TEST_CASE("test read file"){
    string expected = ""
    "Can all this text be read?\n"
    "\n"
    "Will it respect the \\n?";

    Reader* file_reader = new FileReader();
    string read = file_reader->read("src/resources/read_file_test.txt");
    
    REQUIRE(read == expected);
};

TEST_CASE("test includer"){
    string code = ""
    "let x = 0;"
    "a = 2;"
    ;

    Reader* mock_reader = new MockReader(code);
    Includer* includer = new Includer(mock_reader);

    incl_result res  = includer->include("fake_path");
    Environment* env = get<0>(res);

    if(env == 0){
        vector<string> parser_errors = get<1>(res);
        if(parser_errors.size()>0){
            INFO("Parser errors: ");
            for(string err : parser_errors){
                INFO(err);
            }
            FAIL();
        }
        Error* error = get<2>(res);
        if(error != 0){
            INFO("Evaluation error: ");
            INFO(error->message);
            FAIL();
        }
    }

    bool ok = true;
    test_integer_object(env->get("x", ok), 0);
    test_integer_object(env->get("a", ok), 2);
};