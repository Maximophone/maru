# ============================================
# Compute derivatives of functions with MARU!
# ============================================

sum = fn(arr){
    s = 0;
    for(el in arr){
        s = s + el;
    }
    return s;
};

Expression = class{
    value = 0;
    dependencies = "";
    grad_value = "";
    name = "";

    fn(val, name){
        self.value = val;
        self.dependencies = [];
        self.name = name;
    };

    __add__ = fn(other){
        z = Variable(self.value + other.value, self.name + "+" + other.name);
        append(self.dependencies, [1, z]);
        append(other.dependencies, [1, z]);
        return z;
    };

    __mul__ = fn(other){
        z = Variable(self.value * other.value, self.name + "*" + other.name);
        append(self.dependencies, [other.value, z]);
        append(other.dependencies, [self.value, z]);
        return z;
    };
};

Variable = class(Expression){
    grad = fn(){
        if(self.grad_value == ""){
            array = [];
            for(weight_var in self.dependencies){
                weight = weight_var[0];
                var = weight_var[1];
                v_grad = var.grad();
                array = append(array, weight*v_grad);
            };
            self.grad_value = sum(array);
        };
        return self.grad_value;
    };
};

Constant = class(Expression){
    fn(val, name){
        self.value = val;
        self.dependencies = [];
        self.name = name;
        self.grad_value = 1;
    };

    grad = fn(){
        return self.grad_value;
    };

};

grad = fn(x){
    deepest_dependency_grad_value = fn(y){
        for(dependency in y.dependencies){
            var = dependency[1];
            if(len(var.dependencies) == 0){
                var.grad_value = 1;
                return 0;
            }
            else {
                deepest_dependency_grad_value(var);
            }
        }
    }
    deepest_dependency_grad_value(x);
    return x.grad();
};

x = Variable(5, "x");
y = Constant(10, "y");

f = x*x+y

repr(grad(x));
printl()
