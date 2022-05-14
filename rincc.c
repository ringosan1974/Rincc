#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//トークンの種類
typedef enum {
    TK_RESERVED,    //記号
    TK_NUM,         //数値
    TK_EOF,         //入力の終わり
} TokenKind;

typedef struct Token Token;

struct Token{
    TokenKind kind; //トークンの種類
    Token *next;    //次の入力トークン
    int val;        //KindがTK_NUMの場合，その数値
    char *str;      //トークンの文字列
    int len;
};

typedef enum {
    ND_ADD, //加算
    ND_SUB, //減算
    ND_MUL, //乗算
    ND_DIV, //除算
    ND_EQ,  //イコール
    ND_NEQ, //ノットイコール
    ND_NUM, //整数
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;  //ノードの種類
    Node *lhs;      //左辺
    Node *rhs;      //右辺
    int val;        //kindがND_NUMの場合のみ使う
};

void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
bool startswith(char *p, char *q);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *expr();
Node *equality();
Node *add();
Node *mul();
Node *unary();
Node *primary();

//現在着目してるトークン
Token *token;

//入力されたプログラム
char *user_input;

//エラー箇所を報告する
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");   //pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

//次のトークンが記号opのときには，トークンを一つ読み進めてtrueを返す。それ以外の場合にはfalseを返す。
bool consume(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) {
        return false;
    }
    token = token->next;
    return true;
}

//次のトークンが記号opのときには，トークンを一つ読み進める。
//それ以外の場合にはエラーを報告
void expect(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) {
        error_at(token->str, "'%s'ではありません", op);
    }
    token = token->next;
}

//次のトークンが数値の場合，トークンを一つ読み進めてその数値を返す。
//それ以外の場合にはエラーを報告
int expect_number() {
    if (token->kind != TK_NUM) {
        error_at(token->str, "数ではありません");
    }
    int val = token->val;
    token = token->next;
    return val;
}

//入力されたコードの終わりであるか否かを返す
bool at_eof() {
    return token->kind == TK_EOF;
}

//新しいトークンを作成してcurにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

//文字列pが文字列qで始まっているかをboolで返す
bool startswith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

//入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        //空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        } else if (startswith(p, "==") || startswith(p, "!=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        } else if (strchr("+-*/()", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        } else if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }
        
        error_at(token->str, "トークナイズできません");
    }
    
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

//新しいノードを生成
Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

//新しいノードを生成（数値の場合）
Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

//抽象構文木を生成
Node *expr() {
    Node *node = equality();
}

Node *equality() {
    Node *node = add();

    for (;;)
    {
        if (consume("==")) {
            node = new_node(ND_EQ, node, add());
        } else if (consume("!=")) {
            node = new_node(ND_NEQ, node, add());
        } else {
            return node;
        }
    }
    
}

Node *add() {
    Node *node = mul();

    for (;;)
    {
        if (consume("+")) {
            node = new_node(ND_ADD, node, mul());
        } else if (consume("-")) {
            node = new_node(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}

Node *mul() {
    Node *node = unary();

    for (;;)
    {
        if (consume("*")) {
            node = new_node(ND_MUL, node, unary());
        } else if (consume("/")) {
            node = new_node(ND_DIV, node, unary());
        } else {
            return node;
        }
    }
}

Node *unary() {
    if (consume("+"))
    {
        return primary();
    } else if (consume("-")) {
    
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    return primary();
}

Node *primary() {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }
    return new_node_num(expect_number());
}

//抽象構文木を下りながら構文木の左端からコード生成
void gen(Node *node) {
    if (node->kind == ND_NUM)
    {
        printf("    push %d\n", node->val);
        return;
    }
    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind)
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
    }

    printf("    push rax\n");
}

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
