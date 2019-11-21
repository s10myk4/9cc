#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//tokenize.c
//kinds of token
typedef enum {
  TK_RESERVE, //記号
  TK_IDENT, //識別子
  TK_NUM, //整数トークン
  TK_EOF, //入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int length;
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
bool expect(char *op);
int expect_number(void);
bool at_eof(void);
Token *tokenize(void);

//current token
extern Token *token;
//input program
extern char *user_input;

// parse.c
typedef enum {
  ND_ADD, //+
  ND_SUB, //-
  ND_MUL, //*
  ND_DIV, // /
  ND_EQ,  // ==
  ND_NE,  // !=
  ND_LT,  // <
  ND_LE,  // <=
  ND_NUM, // integer

} NodeKind;

//AST node type
typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *next;
  Node *lhs;
  Node *rhs;
  int val; //use if kind == ND_NUM
};

Node *program(void);

// codegen.c
void codegen(Node *node);