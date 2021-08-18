#include "headers.h"
#include "functions.h"

vector<string> dirList;
string currentPath,home;
ofstream myfile;
struct termios init_config, new_config;
struct winsize terminal_dimensions;

int directory_entries;


int openDirectory(string path)
{
    struct dirent *directory_entry;
    // directory points to the directory opened by opendir(directory path)
    DIR *directory;
    //file_attributes is a structure of stat type that holds the information/stats associated with the file
    
    const char *c_path = path.c_str();
    directory=opendir(c_path);
    int count=0;
    dirList.clear();
    while((directory_entry=readdir(directory))!=nullptr)
    {
		
		dirList.push_back(string(directory_entry->d_name));
		count++;
			
    }
    closedir(directory);

    sort(dirList.begin(),dirList.end());
    directory_entries=count;

    //getting terminal dimensions for deciding how much to display without scrolling
    

    
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal_dimensions);
    //current no of lines that can be displayed
    terminal_row=terminal_dimensions.ws_row - 1;
    //current no of chars that can be accomodated in a single line
    terminal_col=terminal_dimensions.ws_col;

    max_display_without_scroll=(directory_entries<=terminal_row)?directory_entries:terminal_row;

    //wintrack=0;

    //cout<<"\033[H\033[J"; //Clear screen from cursor pos down
    cout<<"\033[2J";
    moveCursor(0,0);
    //cout<<endl;
    for (int i = 0; i < directory_entries; i++)
	{
		char *tempFileName = new char[dirList[i].length() + 1];
		strcpy(tempFileName, dirList[i].c_str());
		printFileInfo(tempFileName);
	}
    //printf("%c[%d;%dH", 27, 1, 80);
    

    return 1;
}

string getPath(string name){
    string path=currentPath;
    path.append("/");
    path.append(name);
    return path;
}

void printFileInfo(const char *filename)
{
    
    string path = getPath(filename);
    struct stat file_attributes;
    //to determine file owner
    struct passwd *owner;
    //to determine group
    struct group *g;
    stat(path.c_str(),&file_attributes);
 //setting permissions by testing different rights
    (S_ISDIR(file_attributes.st_mode))? cout<<"d": cout<<"-";
    (file_attributes.st_mode & S_IRUSR)? cout<<"r": cout<<"-";  // user read
    (file_attributes.st_mode & S_IWUSR)? cout<<"w": cout<<"-";   // user write
    (file_attributes.st_mode & S_IXUSR)? cout<<"x": cout<<"-";   // user execute
    (file_attributes.st_mode & S_IRGRP)? cout<<"r": cout<<"-";   // group read
    (file_attributes.st_mode & S_IWGRP)? cout<<"w": cout<<"-";   // group write
    (file_attributes.st_mode & S_IXGRP)? cout<<"x": cout<<"-";   // group execute 
    (file_attributes.st_mode & S_IROTH)? cout<<"r": cout<<"-";   // others read
    (file_attributes.st_mode & S_IWOTH)? cout<<"w": cout<<"-";   // others write
    (file_attributes.st_mode & S_IXOTH)? cout<<"x": cout<<"-";   // others execute

        //owner info
    owner=getpwuid(file_attributes.st_uid);
    //group info
    g=getgrgid(file_attributes.st_gid);
    //size info
    off_t size=file_attributes.st_size;
    unsigned long long int s=size;
    //last modified info
    string last_modified_time=ctime(&file_attributes.st_mtime);
    last_modified_time=last_modified_time.substr(4,12);
    
    //printing all the remaining file attributes
    cout<<"\t"<<owner->pw_name;
    cout<<"\t"<<g->gr_name;
    cout<<"\t"<<size<<"B";
    cout<<"\t" + last_modified_time;

        if(S_ISDIR(file_attributes.st_mode))
        {
            cout<<"\033[1;40m"; // Highlight text if it is a directory
            cout<<"\t" + string(filename);
            cout<<"\033[0m";
        }
    else
        cout<<"\t" + string(filename);

}