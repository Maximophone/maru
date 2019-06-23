#include "catch.hpp"
#include "../object.hpp"

TEST_CASE("test string hash key"){
    String* hello1 = new String("hello world");
    String* hello2 = new String("hello world");
    String* diff1 = new String("different");

    HashKey h1 = hash_key(hello1);
    HashKey h2 = hash_key(hello2);
    HashKey d1 = hash_key(diff1);
    CHECK(h1 == h2);
    CHECK(h1 != d1);
}