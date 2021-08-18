#include "headers.h"
#include "functions.h"
ofstream myfile; 

 
extern string currentPath,home;
extern vector<string> results, printresults;
extern stack<string> backstack;
extern struct winsize terminal_dimensions;


vector<string> command_tokens;

int terminal_row,terminal_col;


stack<string> forw_stack;

vector<string> dirList;
int directory_entries;
int wintrack;
int searchflag = 0;
int max_display_without_scroll;










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