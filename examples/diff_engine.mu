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

    add = fn(other){
        z = Variable(self.value + other.expression.value, self.name + "+" + other.expression.name);
        append(self.dependencies, [1, z]);
        append(other.expression.dependencies, [1, z]);
        return z;
    };

    mul = fn(other){
        z = Variable(self.value * other.expression.value, self.name + "*" + other.expression.name);
        append(self.dependencies, [other.expression.value, z]);
        append(other.expression.dependencies, [self.value, z]);
        return z;
    };
};

Variable = class{
    expression = 0;

    fn(value, name){
        self.expression = Expression(value, name);
    };

    grad = fn(){
        if(self.expression.grad_value == ""){
            array = [];
            for(weight_var in self.expression.dependencies){
                weight = weight_var[0];
                var = weight_var[1];
                v_grad = var.grad();
                array = append(array, weight*v_grad);
            };
            self.expression.grad_value = sum(array);
        };
        return self.expression.grad_value;
    };

    add = fn(other){self.expression.add(other)};
    mul = fn(other){self.expression.mul(other)};
};

Constant = class{
    expression = 0;

    fn(value, name){
        self.expression = Expression(value, name);
        self.expression.grad_value = 1;
    };

    grad = fn(){
        return self.expression.grad_value;
    };

    add = fn(other){self.expression.add(other)};
    mul = fn(other){self.expression.mul(other)};
};

grad = fn(x){
    deepest_dependency_grad_value = fn(y){
        for(dependency in y.expression.dependencies){
            var = dependency[1];
            if(len(var.expression.dependencies) == 0){
                var.expression.grad_value = 1;
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

f = x.mul(x).add(y);

repr(grad(x));
printl()