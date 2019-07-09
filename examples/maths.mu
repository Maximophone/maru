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

test("V2"){
    v1 = V2(2,2);
    v2 = V2(0,0);
    (v1 + v2).x == 2;
};

test("V2 other test"){
    v1 = V2(2,2);
    v2 = V2(0,0);
    v1/v2;
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

    __add__ = fn(other){
        if(type(other)=="INSTANCE"){
            new_elements = [];
            i = 0;
            for(x in self.elements){
                append(new_elements, x + other.elements[i]);
                i = i + 1;
            }
            return array(new_elements);
	}
	if(type(other)=="INTEGER"){
	    new_elements = [];
	    for(x in self.elements){
		append(new_elements, x + other);
	    }
	    return array(new_elements);
	}
    };

    __sub__ = fn(other){
        if(type(other)=="INSTANCE"){
            new_elements = [];
            i = 0;
            for(x in self.elements){
                append(new_elements, x - other.elements[i]);
                i = i + 1;
            }
            return array(new_elements);
	}
	if(type(other)=="INTEGER"){
	    new_elements = [];
	    for(x in self.elements){
		append(new_elements, x - other);
	    }
	    return array(new_elements);
	}
    };
};
