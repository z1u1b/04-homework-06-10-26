#include <unistd.h>
#include <sys/wait.h>
#include <cassert>
#include <cstdio>
#include <string>
#include <iostream>

const char msg[256] = "user data\n";

size_t send(int& err, int wr, const char* b, size_t k)
{
  size_t r = 0;
  while (r < k) {
    err = write(wr, b + r, k - r);
    if (err < 0)
      break;
    r += err;
  }
  return r;
}

int main()
{
  std::string s;
  std::getline(std::cin, s);

  int pps[2] = {}, err = pipe(pps);
  assert(!err);
  int rd = pps[0], wr = pps[1];
  pid_t pid = fork();
  assert(pid >= 0);
  if (!pid) {
    err = close(wr);
    assert(!err);
    char p[100] = {};
    err = sprintf(p, "%d", rd);
    assert(err > 0);
    execl("child", "child", p, NULL);
    assert(0);
  }
  err = close(rd);
  assert(!err);

  char p[8] = {};
  err = sprintf(p, "%zu", s.size());
  assert(err > 0);
  send(err, wr, p, 8);
  assert(err > 0);

  if (!s.empty()) {
    send(err, wr, s.c_str(), s.size());
  }
  err = close(wr);
  assert(!err);
  err = waitpid(pid, 0, 0);
  assert(err == pid);
}
