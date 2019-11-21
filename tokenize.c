#include "9cc.h"

char *user_input;
Token *token;
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
bool consume(char *op) {
  if (token->kind != TK_RESERVE || strlen(op) != token->length ||
      memcmp(token->str, op, token->length))
    return false;

  token = token->next;
  return true;
}

bool expect(char *op) {
  if (token->kind != TK_RESERVE || strlen(op) != token->length ||
      memcmp(token->str, op, token->length))
    error_at(token->str, "expected \"%s\"", op);
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
static Token *new_token(TokenKind kind, Token *cur, char *str, int length) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->length = length;
  cur->next = tok;
  return tok;
}

static bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
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

    //Multi letter punctuator
    if (startswith(p, "==") || startswith(p, "!=") ||
        startswith(p, "<=") || startswith(p, ">=")) {

      cur = new_token(TK_RESERVE, cur, p, 2);
      p += 2;
      continue;
    }

    //single letter punctuator
    if (strchr("+-*/()<>;", *p)) {
      cur = new_token(TK_RESERVE, cur, p++, 1);
      continue;
    }

    //integer literal
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->length = p -q;
      continue;
    }
    error_at(p, "invalid token");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}
