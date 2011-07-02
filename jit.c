#include <chicken.h>
#include <malloc.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>

char *binary;
char *heap;
u_int64_t result;

u_int64_t exec_binary (int len, char *code) {
  memcpy (binary, code, len);
  printf ("current heap block at %Lx\n", heap);
  result = ((u_int64_t (*) (char*, int)) binary) (heap+10, 4096);
  binary += len;
  heap   += 256;
  return 0;
}

u_int32_t fetch_result_higher32 () {
  return (u_int32_t) (result >> 32);
}

u_int32_t fetch_result_lower32 () {
  return (u_int32_t) result;
}

u_int32_t binary_address_higher32 () {
  u_int32_t ret = (u_int32_t) (((u_int64_t) binary)  >> 32);
//  printf ("higher:%x\n", ret);
  return ret;
}

u_int32_t binary_address_lower32 () {
  u_int32_t ret = (u_int32_t) (u_int64_t) binary;
//  printf ("lower:%x\n", ret);
  return ret;
}

void prepare (char* filename) {
  int fd    = open (filename, O_RDWR);
  binary    = (fd != -1) ? mmap (0, 4096, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0)
                         : mmap (0, 4096, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  printf ("code will be loaded at %Lx\n", binary);
  heap      = mmap (0, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  printf ("heap set at %Lx\n", heap);}

int main (int argc, char** argv) {
  prepare (argv[1]);
  C_word k = CHICKEN_run (C_toplevel);}