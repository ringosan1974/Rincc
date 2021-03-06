#include "rincc.h"

//ノードが変数を指している場合、その変数のアドレスを計算してスタックにプッシュ
void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
    {
        error("代入の左辺値が変数ではありません");       
    }
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");
}

//抽象構文木を下りながら構文木の左端からコード生成
void gen(Node *prog) {
    switch (prog->kind)
    {
    case ND_NUM:
        printf("    push %d\n", prog->val);
        return;
    case ND_LVAR:
        gen_lval(prog);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(prog->lhs);
        gen(prog->rhs);
        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
}

    gen(prog->lhs);
    gen(prog->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (prog->kind)
    {
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("    imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        break;
    case ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_NEQ:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_EINE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_INE:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("   movzb rax, al\n");
        break;
    }

    printf("    push rax\n");
}

void codegen() {
    //アセンブリ前半
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    //プロローグ
    //変数26個分の領域を確保する
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, %d\n", stack_size);
    
    for (int i = 0; code[i]; i++)
    {
        gen(code[i]);
        printf("    pop rax\n");
    }
    
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
}