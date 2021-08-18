#include "headers.h"
#include "functions.h"


string createPath(string path, string filename)
{
    string absolutepath=home;
    if(path.compare(".")==0)
    {
        absolutepath=currentPath;
    }
    else if(path[0]=='~')
    {
        absolutepath.append(path.substr(1,path.length()-1));
    }
    else if(path[0]=='/'){
        absolutepath.append(path);
    }
    else{
        absolutepath=currentPath;
        absolutepath.append("/"+path);
    }
    absolutepath.append("/"+filename);

    return absolutepath;
}

void createFileCommand(string filename, string destpath)
{
    string absolutepath=createPath(destpath, filename);
    int fd=open(absolutepath.c_str(), O_WRONLY | O_CREAT, 0644);
    if(fd==-1)
        commands(true);
}


void createDirCommand(string dirname, string destpath)
{
    string absolutepath = createPath(destpath, dirname);
    if(mkdir(absolutepath.c_str(),0775)!=0)
        commands(true);
}