#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/stat.h>
#include<sys/types.h>
#include <signal.h>
#include"def.h"

char tilda[250],proc_name[100][100];
int proc_status[100],proc_count=0,proc_id[100];
void sig_ctrlc()
{
    if(getpid()==parent_pid)
    {
        return;
    }
    kill(getpid(),9);
    signal(SIGINT,sig_ctrlc);
}
void sig_ctrlz()
{
    fz=1;
    signal(SIGTSTP,sig_ctrlz);
}
int main(int argc, const char *argv[])
{
    fz=0;
    parent_pid=getpid();
    realpath(argv[0],tilda);
    tilda_init(tilda);
    signal(SIGTSTP,sig_ctrlz);
    signal(SIGINT,sig_ctrlc);
    while(1)
    {
        char prompt[150],*command,*commands;
        int command_size=350,length,commands_size=150;
        command=(char *)malloc(command_size*sizeof(char));
        commands=(char *)malloc(commands_size*sizeof(char));
        status_bg(proc_status,proc_count,proc_name);
        gen_prompt(prompt,tilda);
        printf("%s",prompt);
        length=getline(&commands,&commands_size,stdin);
        if(strcmp(commands,"\n")==0)
            continue;
        hist_init(commands,tilda);
        char *coms[100],*c;
        c=strtok(commands,";");
        coms[0]=c;
        for (int i = 1; coms[i-1]!=NULL; ++i)
        {
            coms[i]=strtok(NULL,";");
        }
        for (int i = 0; coms[i]!=NULL; ++i)
        {
            strcpy(command,coms[i]);

            int pipe_chk=pipe_run(command,tilda,proc_status,&proc_count,proc_name,proc_id);
            if(pipe_chk!=0)
                continue;

            int redir_check=redir(command);
            /* printf("redir_check = %d\n", redir_check); */
            if(redir_check<0||redir_check==1)
                continue;
            execute_cmd(command,tilda,proc_status,&proc_count,&proc_name,proc_id);
            printf("\n");

        }
    }
}
