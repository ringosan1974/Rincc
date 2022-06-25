#include "rincc.h"

Node *code[100];
LVar *locals = NULL;

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

LVar *find_Lvar(Token *tok) {
    //ポインタがどこを指しているのかがよくわからん
    for (LVar *var = locals; var; var = var->next)
    {
        if (strcmp(var->name, tok->str))
        {
            return var;
        }
    }
    return NULL; 
}

//抽象構文木を生成
Node *program() {
    int i = 0;
    while (!at_eof())
    {
        code[i++] = stmt();
    }
    code[i] = NULL;
    return code[0];
}

Node *stmt() {
    Node *node = expr();
    expect(";");
    return node;
}

Node *expr() {
    return assign();
}

Node *assign() {
    Node *node = equality();
    if (consume("="))
    {
        node = new_node(ND_ASSIGN, node, assign());
    }
    return node;
    
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

    //ここでconsume_ident()の中のTokenのポインタ*tはポインタtoken”自体の”アドレスを指している。
    //なのでtokenの指すアドレスが変わっても*tが指す場所は変わらない。
    Token *tok = consume_ident();
    if (tok)
    {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;
        LVar *lvar = find_Lvar(tok);
        if (lvar)
        {
            node->offset = lvar->offset;
        } else {
            if (locals == NULL) //もしlocalsがヌルポインタだったら0クリアしたLVarを代入しておく
            {
                locals = calloc(1, sizeof(LVar));
            }
            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = locals->offset + 8;
            node->offset = lvar->offset;
            locals = lvar;
            stack_size += 8;
        }
        
        return node;
    }
    
    return new_node_num(expect_number());
}