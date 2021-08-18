#include "headers.h"
#include "functions.h"

extern ofstream myfile;
extern string home;
extern string currentPath;
extern stack<string> pathstack,backstack,forwardstack;
extern vector<string> dirList;
extern struct winsize terminal_dimensions;
extern struct termios init_config, new_config;