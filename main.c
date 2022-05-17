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
    Node *node = expr();

    //アセンブリ前半
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    //スタックトップに式全体の値が残っているはずなので
    //それをraxにロードして関数からの返り値とする
    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}