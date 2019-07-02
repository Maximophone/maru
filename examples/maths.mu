V2 = class{
    x = 0;
    y = 0;
    fn(x, y){
        self.x = x;
        self.y = y;
    };

    __add__ = fn(other){
        return V2(self.x + other.x, self.y + other.y);
    };

    __sub__ = fn(other){
        return V2(self.x - other.x, self.y - other.y);
    };

    __mul__ = fn(scalar){
      return V2(self.x * scalar, self.y * scalar);
    };
};

V3 = class{
    x = 0;
    y = 0;
    z = 0;
    fn(x, y, z){
        self.x = x;
        self.y = y;
        self.z = z;
    };
};

array = class{
    elements = -1;
    fn(elements){
        if(type(elements)!="ARRAY"){
            printl("wrong type, needs array");
        }
        self.elements = elements;
    };

    sum = fn(){
        s = 0;
        for(x in self.elements){
            s = s + x;
        }
        return s;
    };
};