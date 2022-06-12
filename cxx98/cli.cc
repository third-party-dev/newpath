
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <clocale>

using namespace std;

/* --- Start of unistd.h definitions --- */

typedef long int ssize_t;

#ifdef _WIN32
#define isatty _isatty
#endif
/* Note: Avoiding inclusion of unistd.h for portability. */
extern int isatty (int fd);

/* --- End of unistd.h definitions --- */

void main_usage() {

}

int main(int argc, char *argv[]) {\
  // vector<char> ::iterator iter;
  // vector<char> buf(4);
  // for (iter = buf.begin(); iter != buf.end(); ++iter) {
  //   cout << *iter;
  // }

  char *command = argv[0];

  setlocale(LC_ALL, "");

  if (!isatty(fileno(stdin)))

  while (! cin.eof() ) {
    cout << (unsigned char) cin.get();
  }
}