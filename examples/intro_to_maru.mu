# This is a MARU program.

# Use the printl built in function to print strings.
printl("Hello World!");

# You can also use print if you want to print strings on the same line.
print("Hello ");
print("World!");

# An empty printl goes to the next line
printl();

# You can use repr to represent maru objects that are not strings.
print("Printing a number: ");
repr(2);

# In MARU, the syntax to create variables is simple:
my_var = 1;
a_string_variable = "this is a string";
a_boolean = true;

# In MARU, everything is an expression, even declaring a variable! 
# So you can do things like this:
a = (b = 1);
repr(b);
repr(a);

# Or this
x = 1;
y = 2;
if((a = (x+y)) > 0){
    repr(a);
}

# MARU supports if/else conditionals...
if(true){
    printl("true");
}
else{
    printl("false");
}

# ... which are expressions as well, so you can write
# inline if expressions like so:
x = true;
result = if(x){1}else{2};

# Let's look at the different objects available in MARU
# You can use the "type" builtin function to check an object's type

# BASIC Types
printl(type(1)); # integer
printl(type(true)); # boolean
printl(type("x")); # string

# ARRAYS
# can be arrays of heterogenous types
arr = [1, 2, true, "x"];
printl(type(arr));
repr(arr[2]); # accessing second element
repr(arr[-1]); # accessing undefined elements returns a null object.

# HASHES
hash = {"a": "b", 1: 2, true: false};
printl(type(hash));
repr(hash["a"]);
repr(hash["not here"]); # accessing undefined keys returns a null object.

# Loops
# MARU implements two types of loops: for and while.
# For loops can only be applied on arrays
for(element in arr){
    repr(element);
}

# While loops take a boolean condition
i = 10;
while(i>0){
    repr(i);
    i = i - 1;
}

# FUNCTIONS
# MARU functions are first class objects, meaning they are 
# assigned and manipulated like variables, and can be passed to 
# other functions

my_func = fn(a, b){return a+b;};
repr(my_func(1, 2));

# This function returns a function
adder = fn(x){
    inner = fn(y){
        return x + y;
    };
    return inner;
};

add_7 = adder(7);
repr(add_7(3));

# CLASSES and INSTANCES

# This is the definition of a MARU class
X = class{
    # Attributes and default values
    a = 2; 
    b = 3;

    # The constructor is called just after the class is instantiated
    fn(x, y){
        # self is a special keyword that refers to the instance 
        self.a = x;
        self.b = y;
    };

    # Methods are declared like regular functions
    add = fn(){
        return self.a + self.b;
    };
};
repr(X);


# You instantiate a class by calling it like a function.
# You need to pass arguments to the constructor if you
# defined one.
x = X(1, 1);
repr(x);
repr(x.add());

# INHERITANCE
# A class can inherit from another one like so
Y = class(X){
    c = 5;
    
    # Constructora and Methods can be redifined
    add = fn(){
        return self.a + self.b + self.c;
    };
};
repr(Y);

y = Y(0, 0);
repr(y);
repr(y.add());

# OPERATOR Overloading
# In MARU the operators + - * and / can be overloaded by defining 
# methods called __add__, __sub__, __mul__ and __div__.

V = class{
    x = 0;
    y = 0;

    __add__ = fn(other){
        return V(self.x + other.x, self.y + other.y);
    };

    __sub__ = fn(other){
        return V(self.x - other.x, self.y - other.y);
    };
};

v1 = V(); v1.x = 1; v1.y = 2;
v2 = V(); v2.x = 5; v2.y = 3;

repr(v1);
repr(v2);
repr(v1 + v2);

# BUILTINS
# MARU has a number of builtin functions

# append
arr = [1,2,3];
new_arr = append(arr, 4);
repr(new_arr);

# len
repr(len(new_arr));
repr(len("can be applied on strings as well"));

# range (already seen in loops, creates an array)
arr_1 = range(5);
repr(arr_1);
arr_2 = range(10, 15);
repr(arr_2);