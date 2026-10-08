#include <windows.h>
#include <iostream>
#include <string>
#include <cstdio>

DWORD recv(DWORD& err, HANDLE rd, char* b, DWORD k)
{
  DWORD r = 0;
  bool st = true;
  DWORD h = 0;
  while (r < k) {
    st = ReadFile(rd, b + r, k - r, &h, NULL);
    if (!st) {
      err = GetLastError();
      break;
    }
    r += h;
  }

  return r;
}

int main(int argc, char** argv)
{
  HANDLE rd{reinterpret_cast< HANDLE >(std::stoull(argv[1]))};
  char msg[256] = {};
  DWORD err = 0, k = 255;
  if (recv(err, rd, msg, k) != k) {
    std::cerr << err << '\n';
    CloseHandle(rd);
    return 1;
  }
  CloseHandle(rd);
  printf("%s", msg);
}
