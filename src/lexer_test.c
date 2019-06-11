#include <stdio.h>
#include "minunit.h"
#include "token.h"


int tests_run = 0;
int foo = 5;

static char * test_next_token(){
    char * input = "=*(){},;";
    struct test{
        TokenType expected_type;
        char * literal;
    };
    struct test tests[5] = {
        {ASSIGN, "="},
        {PLUS, "="}
    }
    mu_assert("error, foo != 7", foo ==7);
    return 0;
}

static char * all_tests() {
    mu_run_test(test_next_token);
    return 0;
}

int main(int argc, char **argv){
    char *result = all_tests();
    if (result != 0){
        printf("%s\n", result);
    }
    else {
        printf("ALL TEST PASSED \n");
    }
    printf("Tests run: %d\n", tests_run);
    return result != 0;
}