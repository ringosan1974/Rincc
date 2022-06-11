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

    //アセンブリ前半
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    //プロローグ
    //変数26個分の領域を確保する
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, 208\n");
    
    for (int i = 0; code[i]; i++)
    {
        gen(code[i]);
        printf("    pop rax\n");
    }
    
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
    return 0;
}