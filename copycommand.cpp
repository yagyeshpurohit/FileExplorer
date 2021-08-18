#include "headers.h"
#include "functions.h"

extern ofstream myfile;
string getabsolutepath(string name)
{
    // Assuming "/" denotes the location where application is run
    string absolutepath=home;
    if(name.compare(".")==0){       // given path is "./......"
        absolutepath=currentPath;   // ex: "." == "/home/yagyesh/AOS"
    }
    else if(name[0]=='~'){      // given path is "~/........."
        absolutepath.append(name.substr(1,name.length()-1));    // ex: "~/<fname>" == "/home/yagyesh/AOS/<fname>"
    }
    else if(name[0]=='/'){      //given path is "/....."
        absolutepath.append(name);      // ex: "/<fname>" == "/home/yagyesh/AOS/<fname>"
    }
    else{                           // directly the name of file/directory is given
        absolutepath=currentPath;   
        absolutepath.append("/"+name);  // ex: "/<fname>" == "/home/yagyesh/AOS/<fname>"
    }
    return absolutepath;
}

void copyFile(string source, string destination)
{
    int fd_from, fd_to;
    char buffer[1024];
    long long size;

    if(((fd_from = open(source.c_str(), O_RDONLY)) == -1) || ((fd_to=open(destination.c_str(),O_CREAT|O_WRONLY|O_TRUNC, 0700)) == -1))
        commands(true);
    
       while((size=read(fd_from, buffer, 1024)) > 0)
        {
            if(write(fd_to, buffer, size) != size)
                commands(true);
        
        }
    struct stat sb;
    stat(source.c_str(), &sb);
    chown(destination.c_str(), sb.st_uid, sb.st_gid);
    chmod(destination.c_str(), sb.st_mode);

    close(fd_from);
    close(fd_to);



}

void copyDirectory(string source, string destination)
{
    string sourcepath, destpath;
    
    struct dirent *entry;
    struct stat stat_dir, stat_entry;
    stat(source.c_str(), &stat_dir);
    DIR *d;
    d = opendir(source.c_str());
    if(d)
    {
        while(entry=readdir(d))
        {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                continue;

            struct stat buf;
            stat(sourcepath.c_str(), &buf);

            destpath = destination;
            destpath.append("/").append(entry->d_name);
            if(S_ISDIR(buf.st_mode)!=0)
            {
                
                myfile<<destpath<<endl;
                mkdir(destpath.c_str(),0755);
                copyDirectory(sourcepath, destpath);
            }

            else
            {
                myfile<<"copying from "<<sourcepath<<" to "<<destpath<<endl;
                copyFile(sourcepath, destpath);
                
            }
            
        }
        closedir(d);
    }
}

void copyCommand(vector<string> tokens)
{
    string destination = tokens[tokens.size()-1];   //last element of tokens is the destination directory path
    destination = getabsolutepath(destination);

    for(int i=1;i<tokens.size()-1;++i)
    {
        //get absolute paths for given source files/directories
        string path = getabsolutepath(tokens[i]);
        struct stat buf;
        stat(path.c_str(), &buf);
        // if source is not a directory
        if(S_ISDIR(buf.st_mode)==0)
        {
            string copyfilepath = destination + "/" + tokens[i];
            copyFile(path, copyfilepath);
        }
        //if source is a directory
        else
        {
            copyDirectory(path, destination);
        }
        
    }
}