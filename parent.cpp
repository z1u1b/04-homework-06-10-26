#include <windows.h>
#include <iostream>
#include <string>

const char msg[256] = "user data\n";

DWORD send(DWORD& err, HANDLE wr, const char* b, DWORD k)
{
  DWORD r = 0;
  bool st = true;
  DWORD h = 0;
  while (r < k) {
    st = WriteFile(wr, b + r, k - r, &h, NULL);
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
  HANDLE read, write;

  SECURITY_ATTRIBUTES sa = {
      sizeof(SECURITY_ATTRIBUTES),
      NULL,
      FALSE,
  };
  if (!CreatePipe(&read, &write, &sa, 256)) {
    std::cerr << GetLastError() << std::endl;
    return 1;
  }
  SetHandleInformation(read, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);

  PROCESS_INFORMATION pi = {};
  STARTUPINFOA si = {sizeof(si)};
  std::string cmd = std::string(argv[1]) + ' ' + std::to_string(reinterpret_cast< DWORD_PTR >(read));
  if (!CreateProcessA(argv[1], cmd.data(), NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi)) {
    CloseHandle(read);
    CloseHandle(write);
    std::cerr << GetLastError() << std::endl;
    return 1;
  }
  CloseHandle(read);

  DWORD err = 0, k = 255;
  if (send(err, write, msg, k) != k) {
    std::cerr << err << '\n';
    CloseHandle(write);
    return 1;
  }
  DWORD w = WaitForSingleObject(pi.hProcess, INFINITE);
  CloseHandle(write);
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
}
