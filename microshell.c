#define _XOPEN_SOURCE 500 //for gethostname() from unistd.h when compilling with -std=c99
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_INPUT 255
#define GREEN(string) "\x1b[32m" string "\x1b[0m" //green color
#define BLUE(string) "\x1b[34m" string "\x1b[0m" //blue color

char * getUserInfo();
char * setPath();
char * getCurrentPath();
void cd(char path[]);
void makeDir(char directory[],int param);
void cp(char filefrom[],char fileto[]);
void help(int com_number);
int execFromPath(char** ptr,int count);
int main() {
    char user_input[MAX_INPUT];
    char *user_host_name = getUserInfo();
    char *user_path = setPath();
    char * curr_path;
    char *token;
    char splitted_string[4][30] = {'\0'};
    int i; //to split user input

    while(1)
    {
        curr_path = getCurrentPath();
        fprintf(stdout,GREEN("%s")BLUE(":%s")"$ ",user_host_name,curr_path);
        fgets(user_input, MAX_INPUT, stdin);

        user_input[strcspn(user_input, "\n")] = '\0';
        token = strtok(user_input, " ");
        i = 0;
        while(token!=NULL){                             //string split
            strcpy(splitted_string[i],token);
            i++;
            token = strtok(NULL," ");
        }
        if((strcmp(splitted_string[0],"help")==0 || strcmp(splitted_string[1],"--help")==0)
           && strlen(splitted_string[0])!=0)
        {
            if(strlen(splitted_string[1])==0) //help
            {
                help(0);
            }
            else  //help [command]
            {
                if(strcmp(splitted_string[1],"pwd")==0 || strcmp(splitted_string[0],"pwd")==0) //help pwd or pwd --help
                {
                    help(1);
                }
                else if(strcmp(splitted_string[1],"cd")==0 || strcmp(splitted_string[0],"cd")==0) //help cd or cd --help
                {
                    help(2);
                }
                else if(strcmp(splitted_string[1],"cp")==0 || strcmp(splitted_string[0],"cp")==0) //help cp or cp --help
                {
                    help(3);
                }
                else if(strcmp(splitted_string[1],"mkdir")==0 || strcmp(splitted_string[0],"mkdir")==0) //help mkdir or mkdir --help
                {
                    help(4);
                }
                else if(strcmp(splitted_string[1],"exit")==0 || strcmp(splitted_string[0],"exit")==0) //help exit or exit --help
                {
                    help(5);
                }
                else
                {
                    help(-1);
                }
            }
            memset(splitted_string,'\0',sizeof(splitted_string)); //make splitted_string empty
            continue;
        }

        else if(strcmp(splitted_string[0],"pwd")==0){
            curr_path = getCurrentPath();
            printf("%s" ,curr_path);
            printf("%s\n",curr_path);
        }
        else if(strcmp(splitted_string[0],"cd")==0){
            if(strlen(splitted_string[1])==0 || strcmp(splitted_string[1],"~")==0){
                curr_path  =setPath();
            }
            else{
                cd(splitted_string[1]);
            }
        }
        else if(strcmp(splitted_string[0],"cp")==0 &&
                strlen(splitted_string[1])!=0 && strlen(splitted_string[2])!=0)
        {
            if(strcmp(&splitted_string[2][strlen(splitted_string[2])-1],"/")==0)
            {
                char path_destination[100];
                snprintf(path_destination,sizeof(path_destination),"%s%s",splitted_string[2],splitted_string[1]); //string concatenation
                cp(splitted_string[1],path_destination);
            }
            else
            {
                cp(splitted_string[1],splitted_string[2]);
            }

        }
        else if(strcmp(splitted_string[0],"mkdir")==0 && strlen(splitted_string[1])!=0)
        {
            if(strcmp(splitted_string[1],"-p")==0 && strlen(splitted_string[2])!=0)
            {
                makeDir(splitted_string[2],1);
            }
            else
            {
                makeDir(splitted_string[1],0);
            }
        }

        else if(strcmp(splitted_string[0],"exit")==0){
            break;
        }
        else
        {
            int x = 0;
            char *string_to_exec[5];
            for(int y=0;y<4;y++){
                if(strlen(splitted_string[y])>0){
                    string_to_exec[x] = splitted_string[y];
                    x++;
                }
                string_to_exec[x] = NULL;
            }
            if(execFromPath(string_to_exec,x+1) == 256){
                printf("Command '%s' not found.\n",splitted_string[0]);

            }
        }

        memset(splitted_string,'\0',sizeof(splitted_string)); //make splitted_string empty

    }

    free(user_host_name);
    free(user_path);
    free(curr_path);
    exit(EXIT_SUCCESS);
}


char * getUserInfo(){
    char hostname[MAX_INPUT];
    if(gethostname(hostname,MAX_INPUT-1)!=0){
        perror(NULL);
        printf("ERROR CODE %d",errno);
        return NULL;
    }
    char *username = getenv("USERNAME");
    char *user_host_name = (char*) malloc(2*MAX_INPUT);
    strcat(user_host_name,username);
    strcat(user_host_name, "@");
    strcat(user_host_name, hostname);
    return user_host_name;
}
char * setPath(){
    char *homePath = getenv("HOME");
    if(chdir(homePath)!=0){
        fprintf(stderr,"chdir() to %s failed: %s\n",homePath,strerror(errno));
        return NULL;
    }
    char *buf;
    buf = getcwd(NULL,0);
    if(buf==NULL){
        perror(NULL);
        return NULL;
    }
    return buf;
}
char * getCurrentPath(){
    char * curr_path;
    curr_path = getcwd(NULL,0);
    if(curr_path==NULL){
        perror(NULL);
        return NULL;
    }
    return curr_path;
}
void cd(char path[]){
    if(chdir(path)!=0){
        fprintf(stderr,"chdir() to %s failed: %s\n",path,strerror(errno));
        return;
    }
}
void cp(char filefrom[],char fileto[]){
    FILE *fp_in,*fp_out;
    fp_in = fopen(filefrom,"r");
    fp_out = fopen(fileto,"w");\
    if(fp_in==NULL || fp_out==NULL){
        fprintf(stderr,"Could not open the file %s: %s\n",filefrom,strerror(errno));
        return;
    }
    char line[255];
    while(!feof(fp_in))
    {
        fgets(line,255,fp_in);
        fprintf(fp_out,"%s",line);
    }

}
void makeDir(char directory[],int param){
    if(param==1)
    {
        char dire[4][30];
        char *tok;
        int i = 0;
        tok = strtok(directory,"/");
        while (tok!=NULL)
        {
            strcpy(dire[i],tok);
            i++;
            tok = strtok(NULL,"/");
        }
        char new_dir[100] ={'\0'};
        strcpy(new_dir,dire[0]);
        for(int j=0;j<i;j++){
            makeDir(new_dir,0);
            if(j<i)
            {
                strcat(new_dir,"/");
                strcat(new_dir,dire[j+1]);
            }
        }

    }
    else
    {
        if(mkdir(directory,0775)==-1) //rwxrwxr-x
        {
            fprintf(stderr,"Could not create the directory %s: %s\n",directory,strerror(errno));
            return;
        }
        return;
    }

}
int execFromPath(char** ptr,int count)
{
    char *string_to_exec[4];
    for(int y=0;y<count;y++){
        string_to_exec[y] = ptr[y];
    }
    int status;
    pid_t pid = fork();
    if(pid==0)
    {
        if(execvp(string_to_exec[0],string_to_exec)==-1)
        {
            exit(EXIT_FAILURE);
        }
        else
        {
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        wait(&status);
        return status;
    }

}
void help(int com_number) {

    switch (com_number) {
        case 0:
            fprintf(stdout, "Mateusz Kantorski Microshell, version 1.0, release date 2022\n");
            fprintf(stdout, "Type 'help cd' or 'cd --help' to find out more about the function 'cd'.\n");
            fprintf(stdout, "[ arguments...]\n");
            fprintf(stdout, "cd [dir]\n");
            fprintf(stdout, "mkdir [dir]\n");
            fprintf(stdout, "mkdir [-p] [dir]\n");
            fprintf(stdout, "pwd\n");
            fprintf(stdout, "cp [file1] [file2]\n");
            fprintf(stdout, "cp [file1] [dir]\n");
            fprintf(stdout, "help [function name]\n");
            fprintf(stdout, "[function name] --help\n");
            fprintf(stdout, "exit\n");
            break;
        case 1:
            fprintf(stdout, "pwd\n Print the name of the current working directory.\n");
            break;
        case 2:
            fprintf(stdout, "cd [dir]\n");
            fprintf(stdout, "Change the current directory to [dir].\n");
            fprintf(stdout, "The default [dir] is the value of the HOME shell variable.\n");
            fprintf(stdout, "cd ~ or cd change the current directory to default.\n");
            break;
        case 3:
            fprintf(stdout, "cp [source] [destination]\n");
            fprintf(stdout, "Copy [source] to [destination].\n");
            fprintf(stdout, "[destination] can be a directory.\n");
            break;
        case 4:
            fprintf(stdout, "mdkir [option] [directory]\n");
            fprintf(stdout, "Create the [directory], if it does not already exists.\n");
            fprintf(stdout, "To create directories use -p as [option].\n");
            break;
        case 5:
            fprintf(stdout, "exit\n");
            fprintf(stdout, "Exit the shell.\n");
            break;
        default:
            fprintf(stderr, "Error this command does not exists.\n");
            break;
    }

}
