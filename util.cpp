#include "util.h"

#include <iostream>
#ifdef _WIN32
#include <conio.h>
#elifdef __linux__
#include <cstdio>
#include <termios.h>
#include <unistd.h>
#else
#error "only Windows and Linux are supported!"
#endif

void waitForInput() {
  std::cout << "Press any key to continue.";
#if defined(_WIN32)
// TODO test this
#include <conio.h>
  _getch();
#elif defined(__linux__)
  // saving the original cooked terminal
  termios original = {0};

  if (tcgetattr(STDIN_FILENO, &original))
    perror("tcgetattr from original");

  // making tha terminal raw asf
  termios raw = original;
  raw.c_lflag &= ~(ICANON | ECHO);

  if (tcsetattr(STDIN_FILENO, TCSANOW, &raw))
    perror("tcsetattr to raw");

  (void)getchar();

  if (fflush(stdout))
    perror("fflush failure");

  if (tcsetattr(STDIN_FILENO, TCSANOW, &original))
    perror("tcsetattr to original");

#endif
}
