#include "headers.h"
#include "functions.h"


void deleteRecurse(string absolutepath)
{
    struct stat stat_dir, stat_entry;
    struct dirent *entry;
    DIR *d;
    string fullpath;
    stat(absolutepath.c_str(), &stat_dir);
    if ((d = opendir(absolutepath.c_str())) == NULL) {
        commands(true);
    }
    while ((entry = readdir(d)) != NULL)
    {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;

         fullpath = absolutepath;
        fullpath.append("/").append(entry->d_name);
        stat(fullpath.c_str(), &stat_entry);

        if (S_ISDIR(stat_entry.st_mode) != 0) 
        {
            deleteRecurse(fullpath);
            continue;
        }
        if (unlink(fullpath.c_str()) != 0)
            commands(true);
    }
    if (rmdir(fullpath.c_str()) != 0) 
        commands(true);

    closedir(d);
}


void deleteFileCommand(string filename)
{
string path = getabsolutepath(filename);
unlink(path.c_str());
}


void deleteDirCommand(string directoryname)
{
    string absolutepath = getabsolutepath(directoryname);
    deleteRecurse(absolutepath);
    
}