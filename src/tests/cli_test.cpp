#include "catch.hpp"

#include <vector>

#include "../cli.hpp"

TEST_CASE("test lstrip"){
    struct test {
        string input;
        char ch;
        string expected;
    };

    vector<test> tests = {
        {"blahblah", '\n', "blahblah"},
        {"xx test", 'x', " test"},
        {"--wow", '-', "wow"}
    };

    for(test t : tests){
        string result = lstrip(t.input, t.ch);
        REQUIRE(result == t.expected);
    }
};

TEST_CASE("test cli parsing no arguments"){
    CliParser parser;

    char* str_args[1];
    str_args[0] = (char*) "test";

    Arguments args = parser.parse(1, str_args);
    REQUIRE(args.flags.size() == 0);
    REQUIRE(args.int_args.size() == 0);
    REQUIRE(args.float_args.size() == 0);
    REQUIRE(args.string_args.size() == 0);
};

TEST_CASE("test cli parsing flag"){
    CliParser parser;

    parser.add_flag("-c", "no help");
    parser.add_flag("-v", "no help");
    parser.add_flag("--test", "no help");

    char* str_args[3];
    str_args[0] = (char*) "test";
    str_args[1] = (char*) "-c";
    str_args[2] = (char*) "-v";

    Arguments args = parser.parse(3, str_args);
    REQUIRE(args.flags.size() == 3);

    REQUIRE(args.flags["c"]);
    REQUIRE(args.flags["v"]);
    REQUIRE(!args.flags["test"]);
};

TEST_CASE("test cli parsing multi"){
    CliParser parser;

    parser.add_argument("--first", STRING_ARG, "no help");
    parser.add_optional("--second", INT_ARG, "12", "no help either");
    parser.add_optional("--third", FLOAT_ARG, "1.2", "blah");

    char* str_args[5];
    str_args[0] = (char*) "test";
    str_args[1] = (char*) "--first";
    str_args[2] = (char*) "value of first";
    str_args[3] = (char*) "--third";
    str_args[4] = (char*) "2.2";

    Arguments args = parser.parse(5, str_args);
    REQUIRE(args.flags.size() == 0);
    REQUIRE(args.int_args.size() == 1);
    REQUIRE(args.float_args.size() == 1);
    REQUIRE(args.string_args.size() == 1);

    REQUIRE(args.string_args["first"] == "value of first");
    REQUIRE(args.int_args["second"] == 12);
    REQUIRE(args.float_args["third"] == 2.2f);
};

TEST_CASE("test cli positionals"){
    CliParser parser;

    parser.add_argument("--first", INT_ARG, "no help");
    parser.add_positional("pos", STRING_ARG, "blah");
    parser.add_flag("-c", "no help");
    parser.add_positional("pos_2", INT_ARG, "yar");
    parser.add_positional_optional("pos_3", STRING_ARG, "default", "still no help");

    char* str_args[5];
    str_args[0] = (char*) "test";
    str_args[1] = (char*) "value of the first positional";
    str_args[2] = (char*) "--first";
    str_args[3] = (char*) "3";
    str_args[4] = (char*) "2";

    Arguments args = parser.parse(5, str_args);
    REQUIRE(args.flags.size() == 1);
    REQUIRE(args.string_args.size() == 2);
    REQUIRE(args.int_args.size() == 2);

    REQUIRE(args.provided("--first"));
    REQUIRE(args.provided("pos"));
    REQUIRE(args.provided("pos_2"));
    REQUIRE(!args.provided("-c"));
    REQUIRE(!args.provided("pos_3"));

    REQUIRE(args.int_args["first"] == 3);
    REQUIRE(args.string_args["pos"] == "value of the first positional");
    REQUIRE(args.int_args["pos_2"] == 2);
    REQUIRE(args.string_args["pos_3"] == "default");
};

TEST_CASE("test aliases"){
    CliParser parser;

    parser.add_argument("--first", STRING_ARG, "no help");
    parser.add_alias("-f", "--first");

    char* str_args[3];
    str_args[0] = (char*) "test";
    str_args[1] = (char*) "-f";
    str_args[2] = (char*) "value of first";

    Arguments args = parser.parse(3, str_args);
    REQUIRE(args.string_args.size() == 1);

    REQUIRE(args.string_args["first"] == "value of first");
};