//including necessary c libraries
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

//OPERATING SYSTEM USED IS MAC OS BIG SUR VERSION: 11.2.3


// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")

//history array
char s[100][100];
int count=0;

//some function prototypes
void write_in_my_input_file(int count);
int check_env(char* str,char** arr);



// Function to print the Greeting shell during startup

void welcome_message()
{
    clear();
    printf("\n\n\n\n============================================================="
        "=============================================================");
    printf("\n\n\n\t\t\t\t\t$$$$WELCOME TO MANSI'S MINI SHELL$$$$");
    printf("\n\n\t\t\t\t\t-ROLL NUMBER: 200101064-");
    printf("\n\n\n\n============================================================="
        "=============================================================");
    char* username = getenv("USER");
    printf("\n\n\nHello %s, welcome to this mini shell! Please start with the help command to know what all this shell has to offer!\n Enjoy Using :)", username);
    printf("\n");
}

// Function to take input
int takeInput(char* str)
{
    //defining a pointer to char to store the input command
    char* ptr;

    //reading the input after the sign
    ptr = readline("\n||>> ");
    
    //if input is not null, then printing it to the history buffer
    if (strlen(ptr) != 0) {
        add_history(ptr);
        
        //copying the input command to the char string
        strcpy(str, ptr);
       
        //freeing the memory
        free(ptr);
        return 0;
    } else {
        return 1;
    }
}

// Function to print Current Directory.
void printDir()
{
    //defining an array to store the current directory address
    char cwd[2048];
    //getting the current directory and putting it in the array
    getcwd(cwd, sizeof(cwd));
    //printing the current directory on the terminal
    printf("\nCurrent Directory Path : %s", cwd);
}

// Function where the system command is executed
void execArgs(char** parsed)
{
    //printf("\n\n INSIDE EXEC ARGS 1");
    //printf("\n\n INSIDE EXECARGS");
    
    //checking for an environment variable
    int a=0;
    char* arr[2];
    if(parsed[1]!=NULL){
    a=check_env(parsed[1],arr);
    if(a==1){
        //executing the environment variable and setting it as answer in the parsed array
        parsed[1]=getenv(arr[1]);
    }}
    // Forking a child
    pid_t pid = fork();
    //printf("\n\nTHE PARSED ARRAY CONTAINS: %s, %s",parsed[0],parsed[1]);
    if (pid == -1) {
        //printing error
        printf("\nFailed forking child..");
        return;
    } else if (pid == 0) {
        //executing the command
        if (execvp(parsed[0], parsed) < 0) {
            //printing error
            printf("\nThis is not a proper comamnd! Please type help to see the list of commands and functions supported");
        }
        exit(0);
    } else {
        // waiting for child to die
        wait(NULL);
        return;
    }
}

// Function to execute the pipe command
void execArgsPiped(char** parsed, char** parsedpipe)
{
    // 0 is read end, 1 is write end
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0) {
        printf("\nPipe could not be initialized");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf("\nCould not fork");
        return;
    }

    if (p1 == 0) {
        // Child 1 executing..
        // It only needs to write at the write end
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command 1..");
            exit(0);
        }
    } else {
        // Parent executing
        p2 = fork();

        if (p2 < 0) {
            printf("\nCould not fork");
            return;
        }

        // Child 2 executing..
        // It only needs to read at the read end
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedpipe[0], parsedpipe) < 0) {
                printf("\nCould not execute command 2..");
                exit(0);
            }
        } else {
            // parent executing, waiting for two children
            wait(NULL);
            wait(NULL);
        }
    }
}

//function to execute both input as well as output redirection in a single command
void execArgsRe(char** parsedArgs, char** parsedArgsIn)
{
    //firstly we will fork to create a child
    int pid = fork();
    if (pid == -1) {
        //printing error if fork was not successful
        perror("fork");
    } else if (pid == 0) {
        //defining two file pointers fp1 and fp2 to open the input and output files
            FILE *fp1,*fp2;
            
            
            //printf("\n\n\n\n\nARRAY IS  : %s, %s, %s\n\n\n\n\n",parsedArgsIn[0],parsedArgsIn[1],parsedArgsIn[2]);
        
        //opening the file to take input from
        int a=open(parsedArgsIn[1],O_RDONLY);
        
        //redirecting the input so that it is taken from the file opened
        dup2(a, STDIN_FILENO);
        
        //opening the file to write into and setting the stdout to this file
            fp2=freopen(parsedArgsIn[2], "w", stdout);
            //printf("\n\n\n\n\nARRAY IS  : %s, %s, %s\n\n\n\n\n",parsedArgsIn[0],parsedArgsIn[1],parsedArgsIn[2]);
           // scanf("%d",&a);
            //printf("the number is: %d",a);
        
        //printing error if any
            if(fp1==NULL){printf("\n\n\n\n\nfile not opened!\n\n\n\n\n");}
            
        //executing the commands
            execvp(parsedArgs[0], parsedArgs);
            
        
        close(a);
        
        //closing the file pointer
            fclose(fp2);
        
        //redirecting the output back to terminal
            freopen("/dev/tty", "w", stdout);
            
            
        
    //killing the child after execution
        _exit(1);
    } else {
        //waiting for the child to die
        waitpid(pid, 0, 0);
        //free(parsedArgs);
    }
}

//function to execute single input or output redirection
void execArgsIn(char** parsedArgs, char** parsedArgsIn)
{
    //firstly we will fork to create a child
    int pid = fork();
    
    //defining in and out suvh that in=1 shows th epresence of < and out=1 shows the presence of >
    int in=0,out=0;
    
    //checking for the presence of <
    if(parsedArgsIn[1]!=NULL){in=1;}
    
    //checking for the presence of >
    if(parsedArgsIn[2]!=NULL){out=1;}
    
    //printf("\n\n\n\n\nARRAY IS  : %s, %s, %s\n\n\n\n\n",parsedArgsIn[0],parsedArgsIn[1],parsedArgsIn[2]);
    //printf("\n\n\n\n\nPARSED ARGS ARRAY IS : %s, %s, %s\n\n\n\n\n",parsedArgs[0],parsedArgs[1],parsedArgs[2]);
    
    if (pid == -1) {
        //printing error if fork was not successful
        perror("fork");
    } else if (pid == 0) {
        //printf("\n\nHEY, I HAVE REACHED TILL HERE");
        //printf("\n\n in is : %d",in);
        
        
        if (in) { //if '<' char was found in string inputted by user
            
            
           
            close(0);
            //printf("\n\n in is q: %d",in);
            //printf("\ninside the in branch");
            
            //opening the file to take input from
            int a=open(parsedArgsIn[1],O_RDONLY);
            
            //redirecting the input so that it is taken from the file opened
            dup2(a, STDIN_FILENO);
            
            
            //printf("\nopened the file in read mode");
            //if(a<0){printf("\n %s not found!",parsedArgsIn[1]);}
            //freopen("/dev/tty", "w", stdout);
            //printf("\n\nHEY, I HAVE REACHED TILL HERE11");
            
            //executing the command
            int b=execvp(parsedArgs[0], parsedArgs);
            
            //printing error if any
            if(b<0){printf("\nexecution failed");}
            
            //closing the file
            close(a);
            
            //please ignore the code below
            /*
            
            FILE *fp;
            
            
            fp=freopen(parsedArgsIn[1], "a+", stdin);
            
            if(fp==NULL){printf("\n\n\n\n\nfile not opened!\n\n\n\n\n");}
            
            execvp(parsedArgs[0], parsedArgs);
            
            fclose(fp);
            freopen("/dev/tty", "w", stdout);
           */
            
            
            /*
            FILE* fp;
             printf("\n\n in is q: %d",in);
             printf("\ninside the in branch");
            printf("\ninside the in branch");
            fp=fopen(parsedArgsIn[1], "r");
            printf("\nopened the file in read mode");

            fprintf(fp, "\nDiscount: %d$", 33);
            if(fp==NULL){printf("\n\n\n\n\nfile not opened!\n\n\n\n\n");}
            
            execvp(parsedArgs[0], parsedArgs);
            
            fclose(fp);
            
            printf("\ncompleted the file execution in read mode");
           
            
            
            printf("\ncompleted the file execution in read mode");
            int fd0 = open(parsedArgsIn[1], O_RDONLY,0);
                    dup2(fd0, STDIN_FILENO);
                    
            
            execvp(parsedArgs[0], parsedArgs);
            close(fd0);
                perror("execvp");
             
            
            FILE *fp;
            
            printf("\ninside the out branch\n\n ");
            fp=freopen(parsedArgsIn[1], "r", stdin);
            freopen("output.txt", "w", stdout);
            if(fp==NULL){printf("\n\n\n\n\nfile not opened!\n\n\n\n\n");}
            printf("haha");
            execvp(parsedArgs[0], parsedArgs);
            
            fclose(fp);
            freopen("/dev/tty", "w", stdout);*/
            //please ignore the code above
            
        }

        if (out) { //if '>' was found in string inputted by user
            
            
            //defining a file pointer to the file which we need to redirect output to
            FILE *fp;
            
            //opening the file to write into and setting the stdout to this file
            fp=freopen(parsedArgsIn[2], "w", stdout);
            
            //printing error
            if(fp==NULL){printf("\n\n\n\n\nfile not opened!\n\n\n\n\n");}
            
            //executing the command
            execvp(parsedArgs[0], parsedArgs);
            
            //closing the file pointer
            fclose(fp);
            
            //redirecting the output to terminal
            freopen("/dev/tty", "w", stdout);
            
             
        
        }
        //killing the child process
        _exit(1);
    } else {
        //waiting for the child to die
        waitpid(pid, 0, 0);
        //free(parsedArgs);
    }
}

//function to execute the append operation
void execArgsAp(char** parsedArgs, char** parsedArgsAp)
{
    //firstly we will fork to create a child
    int pid = fork();
    
    //checking if fork was successful
    if (pid == -1) {
        //printing an error if unsuccessful
        perror("fork");
    } else if (pid == 0) {

        //defining a file pointer to the file which we need to append
        FILE *fp;
        
        //opening the file to append and setting the stdout to this file
        fp=freopen(parsedArgsAp[1], "a+", stdout);
        
        //printing error
        if(fp==NULL){printf("\n\n\n\n\nfile not opened!\n\n\n\n\n");}
        
        //executing the command
        execvp(parsedArgs[0], parsedArgs);
        
        //closing the file pointer
        fclose(fp);
        
        //redirecting the output to terminal
        freopen("/dev/tty", "w", stdout);
       
        //killing the child process
        _exit(1);
    } else {
        //waiting for the child to die
        waitpid(pid, 0, 0);
        //free(parsedArgs);
    }
}


// help function to print all the functionalities provided by this shell
void help()
{
    puts("\n=====WELCOME TO MY SHELL HELP====="
        "\nCommands supported by this mini shell are :"
         "\n\n"
        "\n>>cd : using various types of cd command, you can change to different directories"
        "\n>>ls : will list all the files in the current working directory"
        "\n>>exit : will exit you from this shell"
        "\n>>history : will display the history of all the commands given right from the starting of the shell"
        "\n>>all other general commands available in LINUX shell "
        "\n>>single level pipe handling"
        "\n>>single input redirection handling"
        "\n>>single output redirection handling"
        "\n>>single input and output redirection handling in a single command"
        "\n>>environment variables handling for the following 4 environment variables: HOME , SHELL , PATH , TERM  "
         "\n====================================="
         );

    return;
}

// Function to execute other commands
int other_command_handler(char** parsed)
{
    int key = 0;
    //making an array of size 4 to store the names of 4 commands
    char* commands[4];
    
//initiallizing the array to name of these commands
    commands[0] = "exit";
    commands[1] = "cd";
    commands[2] = "help";
    commands[3] = "history";

    //finding which command is given out of the 4 commands
    for (int i = 0; i < 4; i++) {
        if (strcmp(parsed[0], commands[i]) == 0) {
            key = i + 1;
            break;
        }
    }

    
    //executing exit if key is 1
            if(key==1){
                printf("\n================================================================================\n");
        printf("\n\t\t\t\t####EXITING THE SHELL####\n\n\t\t\t\t####THANK YOU :) ####\n");
                printf("\n================================================================================\n");
            write_in_my_input_file(count);
            //printf("\n count is: %d",count);
                exit(0);}
    
    //executing cd if key is 2
            if(key==2){
            if(parsed[1]==NULL||strcmp(parsed[1],"~")==0){
                chdir(getenv("HOME"));
            }else{
                chdir(parsed[1]);}
                return 1;}
    
    //executing help if key is 3
            if(key==3){
                      help();
                      return 1;}
   
    //executing history if key is 1
            if(key==4){
            for(int i=0;i<count;i++){
                printf("command %d : %s\n",i+1,s[i+1]);}
                return 1;}
    

    return 0;
}

//function to parse the command contains $ and also identifying if it is there
int check_env(char* str,char** arr)
{
    // duplicating the string str to str1 so that strsep doesnot influence the structure of str
    char* str1=strdup(str);
    //using strsep to get the commands in between the delimeters
    for (int i = 0; i < 2; i++) {
        arr[i] = strsep(&str1, "$");
        if (arr[i] == NULL)
            break;
    }
    
    if (arr[1] == NULL)
        return 0; // returns zero if no $ is found.
    else {
        return 1;// returns 1 if $ is found
    }
    
}
// function for parsing pipe and also identifyinfg if it is there
int parsePipe(char* str, char** strpiped)
{
    // duplicating the string str to str1 so that strsep doesnot influence the structure of str
    char* str1=strdup(str);
    //using strsep to get the commands in between the delimeters
    for (int i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str1, "|");
        if (strpiped[i] == NULL)
            break;
    }
    
    //the final array contains the commands in between the delimeter so that it could be later used appropriately
    if (strpiped[1] == NULL)
        return 0; // returns zero if no pipe is found
    else {
        return 1;// returns 1 if one pipe is found
    }
}

//function to parse the command contains < and also identifying if it is there
int parseIndirection(char* str, char** strpiped)
{
    // duplicating the string str to str1 so that strsep doesnot influence the structure of str
    char* str1=strdup(str);
    //using strsep to get the commands in between the delimeters
    for (int i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str1, "<");
        if (strpiped[i] == NULL)
            break;
    }
    //the final array contains the commands in between the delimeter so that it could be later used appropriately
    //printf("\n\nparsed indirection array is : %s, %s",strpiped[0],strpiped[1]);
    if (strpiped[1]== NULL)
        return 0; // returns zero if no < is found.
   
    else
        return 1;// returns 1 if one < is found
}

//function to parse the command contains > and also identifying if it is there
int parseOutdirection(char* str, char** strpiped)
{
    // duplicating the string str to str1 so that strsep doesnot influence the structure of str
    char* str1=strdup(str);
    //using strsep to get the commands in between the delimeters
    for (int i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str1, ">");
        if (strpiped[i] == NULL)
            break;
    }
    
    //the final array contains the commands in between the delimeter so that it could be later used appropriately
    if (strpiped[1]== NULL)
        return 0; // returns zero if no > is found.
   
    else
        return 1;// returns 1 if one > is found
}

//function to parse the command contains both < and > and also identifying if it is there
int parseRedirection(char* str, char** strpiped)
{
    // duplicating the string str to str1 so that strsep doesnot influence the structure of str
    char* str1=strdup(str);
    //using strsep to get the commands in between the delimeters
    for (int i = 0; i < 3; i++) {
        strpiped[i] = strsep(&str1, "<>");
        if (strpiped[i] == NULL)
            break;
    }
    
    //the final array contains the commands in between the delimeter so that it could be later used appropriately
    if (strpiped[1]!= NULL&&strpiped[2]!= NULL)
        return 1; // returns 1 when both indirection as well as outdirection is possible
   
    else
        return 0;// returns 0 otherwise
}

//function to parse the command containig >> and also identifying if it is there
int parseAppend(char* str, char** strappend)
{
    // duplicating the string str to str1 so that strsep doesnot influence the structure of str
    char* str1=strdup(str);
    //using strsep to get the commands in between the delimeters
    for (int i = 0; i < 2; i++) {
        strappend[i] = strsep(&str1, ">>");
        if (strappend[i] == NULL)
            break;
    }
    strappend[1]=str1;
   //the final array contains the commands in between the delimeter so that it could be later used appropriately
    if (strappend[1]== NULL)
        return 0; // returns zero if no >> is found.
   
    else
        return 1;// returns 1 if one >> is found
}


// function for parsing the space between the command words and finally storing it in the array parsed
void parseSpace(char* str, char** parsed)
{
    // duplicating the string str to str1 so that strsep doesnot influence the structure of str
    char* str1=strdup(str);
    //using strsep to get the commands in between the delimeters
    for (int i = 0; i < 100; i++) {
        parsed[i] = strsep(&str1, " ");

        if (parsed[i] == NULL)
            break;
        
        if (strlen(parsed[i]) == 0)
            i--;
    }
    //the final array contains the commands in between the delimeter so that it could be later used appropriately
    //printf("\n\n\n\n\nPARSED SPACE ARRAY IS : %s, %s, %s\n\n\n\n\n",parsed[0],parsed[1],parsed[2]);
}

//function to parse the 2nd term in the command to filter out any = sign if any
int parse_eq(char* str, char** arr)
{
    
    // duplicating the string str to str1 so that strsep doesnot influence the structure of str
    char* str1=strdup(str);
    //using strsep to get the commands in between the delimeters
    for (int i = 0; i < 2; i++) {
        arr[i] = strsep(&str1, "=");
        if (arr[i] == NULL)
            break;
    }
    
    
    //the final array contains the commands in between the delimeter so that it could be later used appropriately
    if (arr[1]== NULL)
        return 0;
   
    else
        return 1;
}

//function to parse the env variables if setenv can be executed
int parse_env(char* str, char** arr)
{
    // duplicating the string str to str1 so that strsep doesnot influence the structure of str
    char* str1=strdup(str);
    char* arr1[2];
    //using strsep to get the commands in between the delimeters
    for (int i = 0; i < 2; i++) {
        arr1[i] = strsep(&str1, "=");
        if (arr1[i] == NULL)
            break;
    }
    
    //the final array contains the commands in between the delimeter so that it could be later used appropriately
    if (arr1[1]== NULL){
        return 0; // returns zero if setenv cannot be executed
    }
    else{
        parseSpace(arr1[0], arr);
        arr[2]=arr1[1];
        return 1;// returns 1 if setenv can be executed
    }
}

//process string function to process the string
int processString(char* str, char** parsed, char** parsedpipe, char** parsedre, char** parsedin, char** parsedap)
{
//stores the parsed pipe commands
    char* strpiped[2];
    //stores the parsed input redirection commands
    char* strindirection[2];
    //stores the parsed output redirection commands
    char* stroutdirection[2];
    //stores the parsed append commands
    char* strappend[2];
    //stores the parsed output and input redirection commands together in the or case
    char* strdirection[3];
    
    //some flags
    int piped = 0,redirection=0,append=0,indirection=0,outdirection=0;

    //checking for pipe
    piped = parsePipe(str, strpiped);
    
    //checking for input redirection
    indirection = parseIndirection(str, strindirection);
    
    //checking for output redirection
    outdirection = parseOutdirection(str, stroutdirection);
    
    //checking for append
    append = parseAppend(str, strappend);
    
    //checking for input as well as output redirection
    redirection=parseRedirection(str, parsedre);
    
    
    //rewriting the parsedap array with appropriate parsed variables
    parsedap[0]=strappend[0];
    parsedap[1]=strappend[1];
    
    //rewriting the parsedin array with appropriate parsed variables
    if(outdirection){
        strindirection[0]=stroutdirection[0];
    }
    strdirection[0]=strindirection[0];
    strdirection[1]=strindirection[1];
    strdirection[2]=stroutdirection[1];
    parsedin[0]=strdirection[0];
    parsedin[1]=strdirection[1];
    parsedin[2]=strdirection[2];
    
    //printf("\nlook at me before kaand: %s, %s, %s", strdirection[0],strdirection[1],strdirection[2]);
    
    //parsing space and storing the fromt argument in parsed if pipe is found
    if (piped) {
        parseSpace(strpiped[0], parsed);
        parseSpace(strpiped[1], parsedpipe);
    }
    
    //parsing space and storing the fromt argument in parsed if append is found
    if(append){
        parseSpace(parsedap[0],parsed);
    }
    
    //parsing space and storing the fromt argument in parsed if redirection is found
    if(redirection){
        parseSpace(parsedre[0],parsed);
    }else{
        //parsing space and storing the fromt argument in parsed if only one type of redirection is found
    if(indirection||outdirection){
        parseSpace(strdirection[0],parsed);
    }}
    //parsing space and storing the fromt argument in parsed if none of th above is found
    if(piped!=1&&append!=1&&redirection!=1&&indirection!=1&&outdirection!=1){
        parseSpace(str, parsed);}
    
    
    //printf("\nlook at me after kaand: %s, %s, %s", parsedin[0],parsedin[1],parsedin[2]);
    
    //returnig the appro[rite vale to process furthur
    if (other_command_handler(parsed))
    {return 0;}
    if(append)
    {return 4;}
    if(redirection)
    {return 5;}
    if(indirection||outdirection)
    {return 3;}
    if (piped)
    {return 2;}
    
    
    return 1;
    
    
}


//function to write input in the history file after the shell is exited
void write_in_my_input_file(int count)
{
    //defining a file pointer to point to the history.txt file
    FILE *fptr;
    //printf("inside the printing function");
    //opening the file history.txt or creating it if it is not present already. This file will be showing the history of command used during the span of the shell
    fptr = freopen("history.txt","w",stdout);
    
    
    if(fptr==NULL){
        printf("error in opening the file");
    }
    
    //printing the history stored in the history array into the file history.txt
    for(int i=0;i<count;i++){
        printf("input command %d : %s\n",i+1,s[i+1]);}
    
    //closing the file pointer
    fclose(fptr);
    
    //writing the output back to the terminal
    freopen("/dev/tty", "w", stdout);
}

//function to add element into the history array
void add_to_array(char* inputString)
{
    //copying input string into history array
    strcpy(s[count],inputString);
}

//driver code
int main()
{
    //defining arrays to store input string and parsed arguments
    char inputString[1000], *parsedArgs[100];
    //defining array to store parsed pipe arguments
    char* parsedArgsPiped[100];
    //defining array to store input or output redirection arguments
    char* parsedArgsIn[100];
    //defining array to store append arguments
    char* parsedArgsAp[100];
    //defining array to store input and output redirection arguments
    char* parsedArgsRe[100];
    //defining array to store environment parsed arguments in the given command
    char* parsed_env[3];
    char* parsed_equ[2];
    
    
    //defining the the 4 environment variables and enlisting them in an array of size 4, these will be used later to identify if we need to execute setenv function or not.
    char* env_var[4];
    

    env_var[0] = "HOME";
    env_var[1] = "SHELL";
    env_var[2] = "PATH";
    env_var[3] = "TERM";
    
    
    
    //defining a key variable which will take values dependening on the input commands
    int key = 0;
    
    //displaying a welcome message on the initiation of the shell
    welcome_message();

    while (1) {
        //function to print the current working directory
        printDir();
        // function to take input and storing it in the inputString
        if (takeInput(inputString)!=0)
        {continue;}else{
           //incrementing count on each input taken
                ++count;
            
            //adding the input taken ech time to the input array, it will be used to display history later also it will be used to print the history into the history.txt file
                add_to_array(inputString);
                
                
        // processing sring to categorize the command and identify what type of operation is required to execute it properly by setting an appropriate value of key
        key = processString(inputString, parsedArgs, parsedArgsPiped, parsedArgsRe ,parsedArgsIn, parsedArgsAp);
        // processString returns 0 if there is no command
        // or it is a builtin command,
        // key is 1 if it is a simple command
        // key is 2 if it is including a pipe.
        // key is 3 if it is an indirection or outdirection is present
        // key is 4 if it is append
        // key is 5 if both in as well as outdirection i.e. > as well as < is present
        
            
        //execution of a simple command when key is 1
        if (key == 1)
        {
            //defining two flags , flag=0 shows if it is a single word command
            
            int flag=0;
            //flag2=0 shows if the command is not a setenv command
            int flag2=0;
            
            //parsing the 2nd term in the command to get the word without =
            if(parsedArgs[1]!=NULL){
                parse_eq(parsedArgs[1],parsed_equ);
                //initializing flag as 1 to show that the commad is not a single word commmand
                flag=1;
            }
            //printf("\n\nflag is: %d",flag);
            
            //checking if it is a printenv command, and setting flag to 0 in this case so that it could be properly executed by execArgs
            if(parsedArgs[1]!=NULL){
            if(strcmp(parsedArgs[0],"printenv")==0){
                flag=0;
            }}
            
            //processing if flag =1
            if(flag==1){
                //checking if the 2nd word in the command is any of the given enviroment variables
            if(strcmp(parsed_equ[0],env_var[0])==0||strcmp(parsed_equ[0],env_var[1])==0||strcmp(parsed_equ[0],env_var[2])==0||strcmp(parsed_equ[0],env_var[3])==0){
                //if yes then parsing the input string into parts removing =
                int x=0;
                x=parse_env(inputString,parsed_env);
                //checking if the command is setenv and if the parse contained someting to assign after =
                if(strcmp(parsed_env[0],"setenv")==0&&x==1){
                    //executing the setenv command using the inbuilt setenv function
                    setenv(parsed_env[1], parsed_env[2],1);
                    //printing a message to show successful execution
                    printf("\n\n the environment variable has been successfully changed from %s to %s",parsed_env[1],parsed_env[2]);
                    //setting flag2 to 1 to show that it was setenv command was executed
                    flag2=1;
                }
            }else{
                //putting flag as 1 to furtur execute it using normal execArgs
                flag=0;
                //printf("\n\n did not match!!");
            }}
            //printf("\n\nflag is: %d \nflag2 is: %d",flag,flag2);
            //executing the simple command if it is not a setenv command
            if(flag==0&&flag2==0){
                execArgs(parsedArgs);}
        }

            //executing pipe if key is 2
        if (key == 2)
        {execArgsPiped(parsedArgs, parsedArgsPiped);}
        
            //executing input or output redirection if key is 3 but not both at the same time
        if (key==3)
        {execArgsIn(parsedArgs, parsedArgsIn);}
        
            //executing append if key is 4
        if(key==4){
            execArgsAp(parsedArgs, parsedArgsAp);}
        
            //executing input output redirection when both are present in the same command
        if(key==5){
            execArgsRe(parsedArgs, parsedArgsRe);
            }
    }
    }
    
    return 0;
}
