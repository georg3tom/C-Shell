#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>

void status_bg(int *proc_status,int proc_count,char proc_name[][100])
{
    int state;
    int proc_id=waitpid(-1,&state, WNOHANG|WUNTRACED);
    while(proc_id>0)
    {
        if(WIFEXITED(state)||WIFSIGNALED(state))
        {
            for (int i = 0; i<proc_count; ++i)
            {
                if(proc_id==proc_status[i])
                {
                    proc_status[i]=0;
                    printf("[%d] exited\n",proc_id);
                }
            }
        }
        proc_id=waitpid(-1,&state, WNOHANG|WUNTRACED);
    }
}

char p_stat(char *proc_path)
{
    FILE  *p_file=fopen(proc_path,"r");
    char *line=NULL;
    size_t len=1000;
    getline(&line,&len,p_file);
    char word;
    char arRay[1000];
    strcpy(arRay,line);
    for (int i = 0; i < strlen(arRay); ++i)
    {
        if(arRay[i]==')')
        {
            word=arRay[i+2];
            break;
        }
    }
    return word;
}
char *nth_word(char *proc_path,int n,char *ret)
{
    FILE  *p_file=fopen(proc_path,"r");
    char *line=NULL;
    size_t len=1000;
    getline(&line,&len,p_file);
    char *word;
    word= strtok(line, " ");
    for (int i = 1; i < n; ++i)
    {
        word=strtok(NULL," ");
    }
    fclose(p_file);
    strcpy(ret,word);
    return ret;
}
void pinfo(char **arg)
{
    int pid;
    if(arg[1]==NULL)
        pid=getpid();
    else
        pid=strtol(arg[1],(char **)NULL,10);
    char str_pid[10];
    sprintf(str_pid,"%d",pid);
    char proc_path[50]="/proc/",file_read[20];
    strcat(proc_path,str_pid);
    if(access(proc_path,F_OK)==-1)
    {
        printf("Process does not exist\n");
        return;
    }
    strcat(proc_path,"/");
    strcat(proc_path,"stat");
    printf("%s\n",proc_path);
    FILE  *p_file=open(proc_path,O_RDONLY);
    printf("pid:%d\n",pid);
    printf("Process status:%c\n",p_stat(proc_path));
    strcat(proc_path,"m");
    printf("Memory:%s\n",nth_word(proc_path,1,file_read));
    strcpy(proc_path,"/proc/");
    strcat(proc_path,str_pid);
    strcat(proc_path,"/exe");
    char real_path[100];
    realpath(proc_path,real_path);
    printf("Executable path:%s\n",real_path);
}
void jobsppp(int proc_status[],int proc_count,char proc_name[][100],int *proc_id)
{
    int ii=1;
    for (int i = 0; i < 100; ++i)
    {
        if(proc_status[i]!=0)
        {
            int pid=proc_status[i];
            char str_pid[10],status[100];
            sprintf(str_pid,"%d",pid);
            char proc_path[50]="/proc/",file_read[20];
            strcat(proc_path,str_pid);
            if(access(proc_path,F_OK)==-1)
            {
                strcpy(status,"Stopped");
                return;
            }
            else
            {
                strcat(proc_path,"/");
                strcat(proc_path,"stat");
                char c=p_stat(proc_path);
                if(c=='S')
                    strcpy(status,"Sleeping");
                else if(c=='R')
                    strcpy(status,"Running");
                else if(c=='T')
                    strcpy(status,"Stopped");
                else if(c=='Z')
                    strcpy(status,"Zombie");

            }
            printf("[%d] %s %s [%d]\n",ii,status,proc_name[i],proc_status[i]);
            ii++;
        }
    }
}
void kjob(int proc_status[],char **arg)
{
    if(arg[1]==NULL||arg[2]==NULL||arg[3]!=NULL)
    {
        printf("Requires 2 args\n");
        return;
    }
    int id=strtol(arg[1],(char **)NULL,10);
    int signal=strtol(arg[2],(char **)NULL,10);
    int ii=0,pid=-1;
    for (int i = 0; i < 100; ++i)
    {
        if(proc_status[i]!=0)
        {
            ii++;
            if(id==ii)
            {
                pid=proc_status[i];
                break;
            }
        }
    }
    if(pid==-1)
    {
        return;
    }
    kill(pid,signal);
}

void overkill(int proc_status[])
{
    for (int i = 0; i < 100; ++i)
    {
        if(proc_status[i]!=0)
            kill(proc_status[i],9);
    }
}
