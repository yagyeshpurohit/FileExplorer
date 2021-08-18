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


ofstream myfile;
string home;
string currentPath;
stack<string> pathstack,backstack,forwardstack;
vector<string> dirList,results;
struct winsize terminal_dimensions;
struct termios init_config, new_config;
int terminal_row, terminal_col;
int max_display_without_scroll;


int directory_entries;
int cursor = 0;

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

    cout<<endl;

}


//NAVIGATE.CPP
char *currentpath;

void moveCursor(int x,int y) {
	cursor=x;
    cout<<"\033["<<x<<";"<<y<<"H";
    //fflush(stdout);
}

void enableRawMode()    //Enable raw(byte by byte input) mode
{
    tcgetattr(0, &init_config);
    //non canonical mode set
    new_config=init_config;
    new_config.c_lflag &= ~ICANON;
    new_config.c_lflag &= ~ECHO;
    // new_config.c_cc[VMIN]=1;
    // new_config.c_cc[VTIME]=0;
    int setattr = tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_config);
    if(setattr!=0)
        fprintf(stderr, "Could not enable raw mode \n");
}
void disableRawMode()   //Enable canonical(line by line input) mode
{
    tcsetattr(STDIN_FILENO, TCSANOW, &init_config);
}
void navigate(bool flag)
{
    
    enableRawMode();
    
    
    char input;

    while(input=cin.get())
    {
        // int lastline=terminal_row+1;
        // moveCursor(lastline,1);
        // cout<<"Mode: NORMAL MODE";
        // moveCursor(1,1);

        
        if(input==27)   //27 is code for escape(^[)
        {
            input=cin.get();
            input=cin.get();

            if(input=='A') //(input='^[[A'..ie UP arrow key)
            {
                if(cursor>0)
                {
                    cursor--;
                    cout<<"\033[1A";    // move up 1 line
                }
                         

            }

            else if(input == 'B')
				{
                    if(cursor<dirList.size()-1)
                    {
                        cursor++;
                        cout<<"\033[1B";    // move down 1 line
                    }
				}
            
            else if(input == 'C')
            {
                if(!forwardstack.empty())
                {
                    string file=forwardstack.top();
                    forwardstack.pop();
                    backstack.push(file);
                    openDirectory(file);
                    moveCursor(0,0);
                    
                }
            }
            else if(input=='D')    
            {
                if(backstack.size()>1)
                {
                    string file=backstack.top();
                    backstack.pop();
                    forwardstack.push(file);
                    file=backstack.top();
                    openDirectory(file);
                    moveCursor(0,0);
                    
                }
            }
            else
            {
                
            }
        }

        else if(input==127)    //Backspace
        {
            if(currentpath!=home)
            {
                while(!forwardstack.empty())
                {
                    forwardstack.pop();

                }
                currentPath=currentPath.substr(0, currentPath.find_last_of("\\/"));
                openDirectory(currentPath);
                moveCursor(0,0);
            }
        }

        else if(input==104)    //Home key
        {
            backstack.push(home);
            while(!forwardstack.empty())
            {
                forwardstack.pop();
            }
            openDirectory(home);
            moveCursor(0,0);
        }

        else if(input==10) //Enter key
        {
            string file;
            if(flag)
            {
                file=results[cursor];
                flag=false;
            }
            else
                file = dirList[cursor];

            struct stat sb;
            stat(file.c_str(), &sb);
            bool isDirectory=S_ISDIR(sb.st_mode);

            if(isDirectory) // Open directory
            {
                string currentFile=file.substr(file.find_last_of("\\/")+1,file.length());
                if(currentFile.compare(".")==0) // If enter is pressed on "." entry
                {
                    openDirectory(currentPath);
                    moveCursor(0,0);
                }
                else if(currentFile.compare("..")==0)   // If enter is pressed on ".." entry
                {
                    currentPath=currentPath.substr(0, currentPath.find_last_of("\\/"));
                    backstack.push(currentPath);
                    openDirectory(currentpath);
                    moveCursor(0,0);
                }
                else
                {
                    backstack.push(file);
                    openDirectory(file);
                    moveCursor(0,0);
                }
                

            }
            else    // Open a file using fork
            {
                int pid = fork();
                if(pid==0)
                {
                    close(2);
                    execlp("/usr/bin/xdg-open", file.c_str(), nullptr);
                    exit(1);
                }
            }
            
        }

        else if(input=='.')
        {
            commands(false);
        }

        else if(input == 'q')
        {
            printf("\033[?1049l"); //out of alternate buffer
            disableRawMode();
            exit(1);
        }
        else
        {
            
        }
        
    }
    disableRawMode();    

}



vector<string> command_tokens;


void commands(bool fail)
{
//We assume ":" has been pressed by user.
ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal_dimensions);
string command="";
 moveCursor(terminal_dimensions.ws_row,0);

 clearline;
 cout<<":";
 char ch;
 while(ch=cin.get())
 {
     enableRawMode();
    if(ch==10)    //Enter keypress, execute command
    {
        execute(command);
        command="";
    }
      else if(ch==127){
            if(command.size()>0)
            {
                command.pop_back();
                clearline;
                 moveCursor(terminal_dimensions.ws_row,0);
                cout<<":"<<command;
            }
      }    

    else 
    {
        if(ch==27)  //esc keypress, Enter normal mode 
        {
            moveCursor(terminal_dimensions.ws_row,0);
            cout<<"MODE: NORMAL";
            openDirectory(".");
            navigate(false);
            moveCursor(1,1);
        }
        cout<<ch;
        command+=ch;
    }
}


}

void execute(string command)
{
    command_tokens.clear();
    //using strtok() func of c style strings to split complete command in tokens: 
    char *c=strcpy(new char[command.length()+1], command.c_str() );
    char *token = strtok(c, " ");
    while (token != NULL){
        command_tokens.push_back(token);
        token = strtok(NULL, " ");
    }

    
    string commandName=command_tokens[0];

    if(commandName.compare("copy")==0)
    {
        if(command_tokens.size()<=1){
            commands(true);
        }
        
        else
        {
            copyCommand(command_tokens);
            moveCursor(terminal_dimensions.ws_row,0);
            clearline;
            cout<<":";
        }
    }

    else if(commandName.compare("move")==0)
    {
        if(command_tokens.size()<=1){
            commands(true);
        }
        
        else
        {
           //copying multiple files/directories requires sending entire command_tokens vector rather than just sending source and destination
            moveCommand(command_tokens);
            moveCursor(terminal_dimensions.ws_row,0);
            clearline;
            cout<<":";
        }        
    }

    else if(commandName.compare("rename")==0)
    {
        if(command_tokens.size()<=2){
            commands(true);
        }
        
        else
        {
            renameCommand(command_tokens[1],command_tokens[2]);
            moveCursor(terminal_dimensions.ws_row,0);
            clearline;
            cout<<":";
        }        
    }    
    
    else if(commandName.compare("create_dir")==0)
    {
        if(command_tokens.size()<=2){
            commands(true);
        }
        
        else
        {
            createDirCommand(command_tokens[1],command_tokens[2]);
            moveCursor(terminal_dimensions.ws_row,0);
            clearline;
            cout<<":";
        }        
    }      
    
    else if(commandName.compare("create_file")==0)
    {
        if(command_tokens.size()<=2){
            commands(true);
        }
        
        else
        {
            createFileCommand(command_tokens[1],command_tokens[2]);
            moveCursor(terminal_dimensions.ws_row,0);
            clearline;
            cout<<":";
        }        
    }          

    else if(commandName.compare("delete_file")==0)
    {
        if(command_tokens.size()<=1){
            commands(true);
        }
        
        else
        {
            deleteFileCommand(command_tokens[1]);
            moveCursor(terminal_dimensions.ws_row,0);
            clearline;
            cout<<":";
        }        
    }        
    
    else if(commandName.compare("delete_dir")==0)
    {
        if(command_tokens.size()<=1){
            commands(true);
        }
        
        else
        {
            deleteDirCommand(command_tokens[1]);
            moveCursor(terminal_dimensions.ws_row,0);
            clearline;
            cout<<":";
        }        
    }         

    else if(commandName.compare("goto")==0)
    {
        if(command_tokens.size()<=1){
            commands(true);
        }
        
        else
        {
            gotoCommand(command_tokens[1]);
            // moveCursor(terminal_dimensions.ws_row,0);
            // clearline;
            // cout<<":";
        }        
    }    

    else if(commandName.compare("search")==0)
    {
        if(command_tokens.size()>1)
        {
            // results.clear();
            // printresults.clear();
           bool searchresult = search(currentPath, command_tokens[1]);
            cout<<"\033[2J";  //clear entire screen
            backstack.push(currentPath);
            moveCursor(1,1);
            if(searchresult)
                cout<<"Found "<<command_tokens[1];
            else
            {
                cout<<"Not Found "<<command_tokens[1];
            }
            
            moveCursor(1,1);
            navigate(true);
        }
        else
        {
            commands(true);
        }
        
    }  

    else
    {
        commands(true);
    }
                  

}



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

//.......................................................................................................

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

//.....................................................................................................


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

//.........................................................................................

void gotoCommand(string dir)
{
    string absolutepath = home;
    if(dir.compare("/")!=0) 
        absolutepath.append("/"+dir);
    backstack.push(absolutepath);
    openDirectory(absolutepath);
    navigate(false);
}

//.....................................................................................



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

//.......................................................................................

void renameCommand(string oldname, string newname)
{
    string older = getabsolutepath(oldname);
    string newer = getabsolutepath(newname);
}

//.....................................................................................

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