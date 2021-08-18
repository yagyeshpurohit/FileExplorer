#include "headers.h"
#include "functions.h"

extern ofstream myfile;
extern string home;
extern stack<string> backstack,frontstack;

void gotoCommand(string dir)
{
    string absolutepath = home;
    if(dir.compare("/")!=0) 
        absolutepath.append("/"+dir);
    backstack.push(absolutepath);
    openDirectory(absolutepath);
    navigate(false);
}
