#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>   //For file stats like permissions,owner,group,size ....
#include <dirent.h> //For directory structure pointer
#include <unistd.h>
#include <sys/ioctl.h>  //For terminal dimensions
#include <pwd.h>    //For owner info
#include <grp.h>    //For group info
#include <time.h>   //For file modification time info
#include <termios.h>    //For canonical and non canonical(raw) mode
#include <stdlib.h> //For atexit(disableRawMode)
#include <fcntl.h>
#include <string.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <string>
#include <stack>    // for back_stack and forw_stack
#include <algorithm> //for sort function
#include <fstream>
using namespace std;
extern ofstream myfile;
extern string home;
extern string currentPath;
extern stack<string> pathstack,backstack,forwardstack;
extern vector<string> dirList,results;
extern struct winsize terminal_dimensions;
extern struct termios init_config, new_config;
extern int terminal_row, terminal_col;
extern int max_display_without_scroll;