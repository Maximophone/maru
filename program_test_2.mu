SubClass = class{
    a = 0;
    b = 0;
};

NestedClass = class{
    sub_class = SubClass();  
};

nested = NestedClass();

nested.sub_class.a = "x";
nested.sub_class.b = "y";

printl(nested.sub_class.a);

repr(nested);
printl();
