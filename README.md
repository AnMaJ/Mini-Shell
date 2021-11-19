# Mini-Shell
Mini Shell in C ++ implementing the basic command line functionalities 
## Instructions to execute the shell:
1. Download the readline library using the following command one the terminal : $ sudo apt-get install libreadline-dev
Or if you have home-brew installed, directly type : brew install readline
2. Go the directory where you have this c program
3. Compile this c program using the command: gcc 200101064_Assign02.c -lreadline
4. Then, to run the program, type: ./a.out
5. This will execute the mini shell, to exit the shell, give the exit command.
## Salient features of the min shell:
1. Executes all UNIX external commands
2. Single input redirec>on using <
3. Single output redirec>on using >
4. Single append opera>on using >>
5. Both input as well as output redirec>on in a single command for example, command like ./a.out<input.txt>output.txt are handled
6. Single level piping using |
7. 4 environment variables (HOME, SHELL, PATH, TERM) can be executed
especially the commands like :
1. echo $HOME will print the value of the environment variable. 2. printenv HOME will print the value of the environment variable. 3. setenv HOME = xyz will set the environment variable to xyz.
8. cd command and its varia>ons can be used to change the current directory
9. An error message is shown when either a command is incorrect or there was some error in the execu>on
10. For input, output, and append redirec>ons, if the output file is not present, it will be created in the current working directory and the desired output will be found in that output file. Similarly for append.
11. A file named history.txt will be created in the current working directory ager the termina>on of the shell. It will keep the record of all the input commands given to the mini shell during execu>on.
12. Help command will provide the list of all the func>onali>es of the mini shell.
13. history command will give you the list of all the commands executed >ll then in the shell.
14. exit command will take you out of the shell.
15. A welcome header will appear when you will execute the shell and a
Thank you footer will appear when you will exit from the shell.
===========================================================
