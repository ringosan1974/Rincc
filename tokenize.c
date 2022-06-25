#include "rincc.h"

Token *token;

//エラー報告
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

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

//次のトークンが識別子のときには、トークンを一つ読み進めてそのトークンのポインタを返す
Token *consume_ident()  {
    if (token->kind != TK_IDENT)
    {
        return NULL;
    }
    Token *t = token;
    token = token->next;
    return t;    
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

//与えられた文字がアルファベットかアンダースコアかを判別
bool is_alpha(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z' || c == '_');
}

//アルファベットとアンダースコアに加えて数字かも判別
bool is_alnum(char c) {
    return is_alpha(c) || ('0' <= c && c <= '9');
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
        } else if (is_alpha(*p)) {
            char *q = p++;
            while (is_alnum(*p))
            {
                p++;
            }
            cur = new_token(TK_IDENT, cur, q, p - q);
            continue;
        } else if (*p == '=') {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        } else if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "=>") || startswith(p, "=<")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        } else if (strchr("+-*/()><", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        } else if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        } else if (*p == ';') {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }
        
        error_at(token->str, "トークナイズできません");
    }
    
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}