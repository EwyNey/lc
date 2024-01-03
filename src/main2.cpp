#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

typedef int (*sum_f)(int, int);

int main() {
  uint8_t prog[] = {0x8D, 0x04, 0x37, 0xC3};
  void *r = mmap(0, sizeof(prog) * sizeof(uint8_t),
                 PROT_READ | PROT_WRITE | PROT_EXEC,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  memcpy(r, prog, sizeof(prog) * sizeof(uint8_t));

  sum_f sum = (sum_f)r;

  int res = sum(100, 6);

  printf("KEK: %d\n", res);
}