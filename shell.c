/*================================================================
* Filename:shell.c
* Author: KCN_yu
* Createtime:Thu 05 Nov 2020 10:46:46 AM CST
================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <wait.h>
#include <stdbool.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>

#define MAX 64

/////////////////////////////////////////////////////////////////////////////////

void Get_username(){
    struct passwd* pwd;
    pwd = getpwuid(getuid());
    printf("%s@",pwd->pw_name);
}
void Get_hostname(){
    char host[128];
    gethostname(host,sizeof(char)*128-1);
    printf("%s",host);
}
void Get_dir(){
    char dir[128];
    getcwd(dir,sizeof(char)*128);
    printf(":%s",dir);
}
void Print_prefix(){
    Get_username();
    Get_hostname();
    Get_dir();
    printf("# ");
}

/////////////////////////////////////////////////////////////////////////////////

char cmd[MAX][MAX];
int cmd_count = 0;
int fd[2];
bool FLAG_EXEC = true,
     IS_BUITIN = false,
     IS_EXTERNAL = false,
     IS_REDIRECT_IN = false,
     IS_REDIRECT_OUT = false,
     IS_PIPE = false;
const char space[2] = " ";

void Input_command(){
    char* input = (char*)calloc(MAX,sizeof(char));
    char** cmd_tmp = (char**)calloc(sizeof(char*),MAX);
    if(fgets(input,MAX,stdin) != NULL){
        cmd_tmp[cmd_count++] = strtok(input,space);
        while((cmd_tmp[cmd_count] = strtok(NULL,space)) != NULL) cmd_count++;
        cmd_tmp[cmd_count-1][strlen(cmd_tmp[cmd_count-1])-1] = '\0';
    }
    for(int i = 0; i < cmd_count; i++) strcpy(cmd[i],cmd_tmp[i]);
}
void Analysis_command(){
    if(!(strcmp((cmd[0]),"cd")) || !(strcmp(cmd[0],"exit"))) IS_BUITIN = true;
    else{
        for(int i = 0; i < cmd_count; i++){
            if(!strcmp(cmd[i],"|")){
                IS_PIPE = true;
            }
            else if(!strcmp(cmd[i],"<")){
                IS_REDIRECT_IN = true;
            }
            else if(!strcmp(cmd[i],">")){
                IS_REDIRECT_OUT = true;
            }
        }
        IS_EXTERNAL = true;
    }
}
void Run_builtin(){
    if(!strcmp(cmd[0],"exit")) exit(EXIT_SUCCESS);
    else if(!strcmp(cmd[0],"cd")){
        char cd_path[128];
        struct passwd* pwd;
        pwd = getpwuid(getuid());
        if((strlen(cmd[1])) == 0){
            sprintf(cd_path,"/%s",pwd->pw_name);
            strcpy(cmd[1],cd_path);
            cmd_count++;
        }
        else if(strcmp(cmd[1],"~") == 0){
            sprintf(cd_path,"/%s",pwd->pw_name);
            strcpy(cmd[1],cd_path);
        }
        if((chdir(cmd[1])) < 0) fprintf(stderr,"%s: failed\n",cmd[0]);
    }
}
void Run_external(){
    int status;

    char** cmd_tmp = (char**)calloc(sizeof(char*),MAX);

    for(int i = 0; i < cmd_count; i++){
        cmd_tmp[i] = (char*)calloc(MAX,sizeof(char));
        strcpy(cmd_tmp[i],cmd[i]);
    }
    if(fork() == 0){
        if(execvp(cmd_tmp[0],cmd_tmp) < 0){
                fprintf(stderr,"%s: command not found\n",cmd[0]);
                exit(1);
        }
    }
    else{
        wait(&status);
        if(WEXITSTATUS(status)) FLAG_EXEC = false;
    }
}
void Run_command(){
    Analysis_command();
    if(IS_BUITIN) Run_builtin();
    else if(IS_EXTERNAL) Run_external();
}

/////////////////////////////////////////////////////////////////////////////////

void Init(){
    for(int i = 0; i < MAX; i++) strcpy(cmd[i],"\0");
    cmd_count = 0;
    FLAG_EXEC = true,
    IS_BUITIN = false,
    IS_EXTERNAL = false,
    IS_REDIRECT_IN = false,
    IS_REDIRECT_OUT = false,
    IS_PIPE = false;
}
