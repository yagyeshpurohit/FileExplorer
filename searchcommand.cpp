#include "headers.h"
#include "functions.h"

//vector<string> results, printresults;

string home;
struct winsize terminal_dimensions;
ofstream myfile;


bool search(string curpath, string tosearch)
{
    string absolutepath;
    DIR *d;
    struct stat stat_dir, stat_entry;
    struct dirent *entry;

    if ((d = opendir(curpath.c_str())) == nullptr) {
        commands(true);
    }

    while ((entry = readdir(d)) != nullptr)
    {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;

        absolutepath = curpath;
        absolutepath.append("/").append(entry->d_name);
        if(entry->d_name == tosearch)
            return true;
    }
    closedir(d);
    return false;
}