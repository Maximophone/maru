STRINGIFY(
pi = 3.141592653589793;
e = 2.718281828459045;

factorial = fn(n){
    s = 1;
    p = n;
    while(p>1){
	s = s * p;
	p = p -1;
    }
    return s;
};

cos = fn(x){
    return 1 - 0.5*(x2=x*x) + 1/factorial(4)*(x4=x2*x2) - 1/factorial(6)*(x6=x4*x2);
};
)
