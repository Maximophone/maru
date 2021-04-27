print_board = fn(board){
    for(i in range(8)){
        for(j in range(8)){
            if(board[i][j]){
                print("Q ")
            }
            else {
                print(". ")
            }
        }
        printl()
    }
    printl(); printl();
}

test("Print Board"){
    board = [
        [false, true, false, false, false, false, false, false],
        [false, false, false, false, false, false, false, false],
        [false, false, true, false, false, false, false, false],
        [false, false, false, false, false, false, false, false],
        [false, false, false, false, false, false, false, false],
        [false, false, false, false, false, false, false, false],
        [false, false, false, false, false, false, false, false],
        [false, false, false, false, false, false, false, false]
    ]
    print_board(board)
}

conflict = fn(board, row, col){
    for(i in range(row)){
        if(board[i][col]){return true;}
        j = row - i;
        if(0 < col - j + 1){
            if (board[i][col - j]){
                return true;
            }
        }
        if(col + j < 8){
            if(board[i][col+j]){
                return true;
            }
        }
    }
    return false;
}

test("Conflict"){
    assert(conflict(board, 3, 4))
}

and = fn(a, b){
    if(a){
        if(b){
            return true
        }
    }
    return false
}

set_board = fn(board, row, col, val){
    new_board = []
    for(i in range(8)){
        append(new_board, [])
        for(j in range(8)){
            if(and((i==row), (j==col))){
                append(new_board[i], val)
            }
            else {
                append(new_board[i], board[i][j])
            }
        }
    }
    return new_board
}

solve = fn(board, row){
    if(row == 8){
        print_board(board)
        return 0;
    }
    for(i in range(8)){
        if(!conflict(board, row, i)){
            board = set_board(board, row, i, true)
            solve(board, row+1)
            board = set_board(board, row, i, false)
        }
    }
    return 1
}

test("Solve"){
    assert(solve(board, 0) == 1)
}

get_empty_board = fn(){
    board = []
    for(i in range(8)){
        append(board, [])
        for(j in range(8)){
            append(board[i], false)
        }
    }
    return board
}



test("Get empty board"){
    board = get_empty_board()
    print_board(board)
    assert(!board[7][7])
}

test("Setting board"){
    board = get_empty_board()
    board = set_board(board, 3, 4, "test")
    assert(board[3][4] == "test")
    assert(board[3][3] == false)
}

board = get_empty_board()
repr(solve(board, 0));
