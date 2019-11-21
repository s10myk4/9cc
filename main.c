#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2)
    error("%s: invalid number of args", argv[0]);

  //tokenize
  user_input = argv[1];
  token = tokenize();

  //parse
  Node *node = program();

  codegen(node);
  return 0;
}