T = class{
    m = fn(){
        return T();
    };
};

printl("instantiating t");
t = T();

printl("calling t.m");
t.m();

printl("DONE");