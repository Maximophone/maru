let counter = fn(n){
    if(n<1){
        return true;
    } else {
        return counter(n-1);
    }
};

counter(5000);