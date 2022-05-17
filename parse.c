#include "rincc.h"

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
    Node *node = relational();

    for (;;)
    {
        if (consume("==")) {
            node = new_node(ND_EQ, node, relational());
        } else if (consume("!=")) {
            node = new_node(ND_NEQ, node, relational());
        } else {
            return node;
        }
    }
    
}

Node *relational() {
    Node *node = add();
    
    for (;;)
    {
        if (consume("=>")) {
            node = new_node(ND_EINE, add(), node);
        } else if (consume("=<")) {
            node = new_node(ND_EINE, node, add());
        } else if (consume(">")) {
            node = new_node(ND_INE, add(), node);
        } else if (consume("<")) {
            node = new_node(ND_INE, node, add());
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