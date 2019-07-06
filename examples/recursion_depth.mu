# In this program, we check different ways to hit
# MARU's maximum recursion depth (of 1000)

tests = {
    "Function": fn(){
        f = fn(){f();};
        f();
    },
    "Method": fn(){
        X = class{
            f = fn(){self.f();};
        };
        X().f();
    },
    "Constructor": fn(){
        Y = class{
            fn(){Y();};
        };
        Y();
    },
}

test = "Constructor";

print("Testing "); printl(test);

tests[test]();

printl("Failed to hit recursion limit");