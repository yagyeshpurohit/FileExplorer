#include "headers.h"


using namespace std;

int openDirectory(string);
void printFileInfo(const char *);
void navigate(bool);
string getPath(string);
//string getpwd();
void enableRawMode();
void disableRawMode();  
void moveCursor(int,int); //navigate
void commands(bool);
void execute(string);

void gotoCommand(string);
void renameCommand(string , string );
string getabsolutepath(string);
void copyFile(string , string);
void copyDirectory(string , string );
void copyCommand(vector<string>);
void moveCommand(vector<string>);
void deleteRecurse(string);
void deleteFileCommand(string);
void deleteDirCommand(string);
string createPath(string , string);
void createFileCommand(string, string );
void createDirCommand(string , string);
bool search(string , string );
string getPath(string);
#define clearline cout<<"\033[2K"; 
