#ifndef _RINCC_H_
#define _RINCC_H_

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//トークンの種類
typedef enum {
    TK_RESERVED,    //記号
    TK_IDENT,       //識別子
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
    ND_ASSIGN,  //=
    ND_LVAR,	//ローカル変数
    ND_EQ,  //==
    ND_NEQ, //!=
    ND_NUM, //整数
    ND_INE,   //<, >
    ND_EINE,    //<=, >=
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;  //ノードの種類
    Node *lhs;      //左辺
    Node *rhs;      //右辺
    int val;        //kindがND_NUMの場合のみ使う
    int offset;     //kindがND_LVARの場合のみ使う
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
bool startswith(char *p, char *q);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
void gen(Node* node);
void gen(Node* node);

//現在着目してるトークン
extern Token *token;

//入力されたプログラム
extern char *user_input;

#endif
