let a = 2;
let b = 3;

let add = fn(x, y){x+y};

let counter = fn(n){
    let inner = fn(x){
        if (x > n){
            return true;
        } else {
            let  foobar = 9999;
            counter(x + 1);
        }
    };
    return inner;
    let intermediate = fn(){
        return inner(0);
    };
    return intermediate;
};

let c = counter(100);
c();