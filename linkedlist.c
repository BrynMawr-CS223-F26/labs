#include <stdlib.h>
#include <string.h>

struct cake {
  float cost;
  char flavor[16];
  struct cake *next;
};

struct cake makeCake(const char *flavor, float cost) {
  struct cake newCake;
  strncpy(newCake.flavor, flavor, 16);
  newCake.cost = cost;
  newCake.next = NULL;
  return newCake;
}

int main() {
  struct cake cake1 = makeCake("red velvet", 2.00);
  struct cake cake2 = makeCake("chocolate", 1.75);
  struct cake cake3 = makeCake("mocha", 3.50);

  cake1.next = &cake2;
  cake2.next = &cake3;
  // A) draw stack and heap here
}
