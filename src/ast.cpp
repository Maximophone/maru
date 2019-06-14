#include <string>
#include "ast.hpp"

using namespace std;

string Program::token_literal(){
    if(statements.size()>0){
        return statements[0]->token_literal();
    } else {
        return "";
    }
};

string Expression::token_literal(){
    return token.literal;
};

string Statement::token_literal(){
    return token.literal;
};