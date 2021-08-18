#include "headers.h"
#include "functions.h"


void moveCommand(vector<string> tokens)
{
    copyCommand(tokens);

    string destination = tokens[tokens.size()-1];
    destination = getabsolutepath(destination);

    for(int i=1;i<tokens.size()-1;++i)
    {
        string path = getabsolutepath(tokens[i]);
        struct stat stat_dir, stat_entry;
        stat(path.c_str(), &stat_dir);

        if (S_ISDIR(stat_dir.st_mode) == 0) // if it is a file
        {
           deleteFileCommand(path);
        }
        else
        {
            deleteRecurse(path);
        }
    }
}