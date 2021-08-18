#include "headers.h"
#include "functions.h"
string home;

//extern struct winsize terminal_dimensions;



//global area ...................................................



//..................................................................

//function declarations..............................................








//.................................................................................
int main()
{
    
    // extern string home,currentPath;
    // extern stack<string> backstack;
    // extern ofstream myfile;
     char buf[50];   
    getcwd(buf,sizeof(buf));
    home="";
    int i=0;
    while(buf[i]!='\0')
    {
        home+=buf[i];
        i++;
    }
    backstack.push(home);
    currentPath=home;
    openDirectory(".");

    navigate(false);

    return 0;
}
