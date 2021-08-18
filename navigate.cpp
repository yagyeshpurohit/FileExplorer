#include "headers.h"
#include "functions.h"
vector<string> dirList, results;
stack<string> backstack,forwardstack;
string currentPath;
ofstream myfile;
string home;
struct winsize terminal_dimensions;
struct termios init_config, new_config;
int cursor = 0;
//................................................................................
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
