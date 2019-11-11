#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

//kinds of token
typedef enum {
  TK_RESERVE, //記号
  TK_NUM, //整数トークン
  TK_EOF, //入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
};

//current token
Token *token;
//input program
char *user_input;

// Reports an error and exit.
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

//report an error location & exit
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

//次のトークンが期待している記号の場合は、トークンを１つ読み進めてtrueを返す
bool consume(char op) {
  if (token->kind != TK_RESERVE || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

bool expect(char op) {
  if (token->kind != TK_RESERVE || token->str[0] != op)
    error_at(token->str, "expected '%c'", op);
  token = token->next;
}

//次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す
int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "expected a number");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

//トークンを作成し、curにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

Token *tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (strchr("+-*/()", *p)) {
      cur = new_token(TK_RESERVE, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(p, "invalid token");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

typedef enum {
  ND_ADD, //+
  ND_SUB, //-
  ND_MUL, //*
  ND_DIV, // /
  ND_NUM, // integer
} NodeKind;

//AST node type
typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val; //use if kind == ND_NUM
};

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

Node *expr();
Node *mul();
Node *primary();
Node *unary();

// expr = mul ("+" mul | "-" mul)*
Node *expr() {
  Node *node = mul();
  for (;;) {
    if (consume('+'))
      node = new_binary(ND_ADD, node, mul());
    else if (consume('-'))
      node = new_binary(ND_SUB, node, mul());
    else
      return node;
  }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul() {
  Node *node = unary();
  for (;;) {
    if (consume('*'))
      node = new_binary(ND_MUL, node, primary());
    else if (consume('/'))
      node = new_binary(ND_DIV, node, primary());
    else
      return node;
  }
}

// primary = "(" expr ")" | num
Node *primary() {
  if (consume('(')) {
    Node *node = expr();
    expect(')');
    return node;
  }
  return new_num(expect_number());
}

// unary = ("+" | "-")? unary
//        | primary
Node *unary() {
  if (consume('+'))
    return unary();
  if (consume('-'))
    return new_binary(ND_SUB, new_num(0), unary());
  return primary();
}

void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
  }

  printf("  push rax\n");
}

int main(int argc, char **argv) {
  if (argc != 2)
    error("%s: invalid number of args", argv[0]);

  //tokenize
  user_input = argv[1];
  token = tokenize();

  //parse
  Node *node = expr();

  //print the first half of assembly
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  //Traverse AST to emit assembly.
  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}