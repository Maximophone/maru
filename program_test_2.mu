let counter = fn(n){
    if(n<1){
        return true;
    } else {
        let foobar = 9999;
        return counter(n-1);
    }
};

counter(1455);
counter(5000);