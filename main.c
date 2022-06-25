#include "rincc.h"

//入力されたプログラム
char *user_input;

int main(int argc, char **argv) {
    if (argc != 2)
    {
        error_at(token->str, "引数の個数が正しくありません");
        return 1;
    }
    
    //トークナイズしてパースする
    user_input = argv[1];
    token = tokenize(user_input);
    program();
    codegen();
    return 0;
}