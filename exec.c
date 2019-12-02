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
int fz=0,parent_pid;
void up(char *cmd_first,char *tilda,int proc_status[],int *proc_count,char proc_name[],int *proc_id)
{
    int ii=0;
    for (int i = 0; i < strlen(cmd_first); ++i)
    {
        if(cmd_first[i]=='\33'&&i+2<strlen(cmd_first)&&cmd_first[i+2]=='A')
        {
            ii++;
        }

    }
    FILE *hist;
    char path[1000];
    strcpy(path,tilda);
    strcat(path,"/.hist");
    hist=fopen(path,"a+");
    char *buff;
    size_t buff_len=0;
    int count=0;
    char history[10][1000];
    int n=10;
    while(getline(&buff,&buff_len,hist)>1&&n--!=0)
    {
        strcpy(history[count++],buff);
        /* printf("%s",buff); */
    }
    hist_init(history[ii-1],tilda);
    int pipe_chk=pipe_run(history[ii-1],tilda,proc_status,&proc_count,proc_name,proc_id);
    if(pipe_chk!=0)
        return;

    int redir_check=redir(history[ii-1]);
    /* printf("redir_check = %d\n", redir_check); */
    if(redir_check<0||redir_check==1)
        return;
    execute_cmd(history[ii-1],tilda,proc_status,proc_count,proc_name,proc_id);
    return;
}

void fg_id(char *cmd_first,char **args,int proc_status[] , int *proc_count)
{
    if(args[1]==NULL||args[2]!=NULL)
    {
        return;
    }
    int pid=strtol(args[1],(char **)NULL,10),ii=0;
    for (int i = 0; i < 100; ++i)
    {
        if(proc_status[i]!=0)
        {
            ii++;
            if(pid==ii)
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
    kill(pid,18);
    waitpid(-1, NULL, WUNTRACED);

}
void bg_id(char *cmd_first,char **args,int proc_status[] , int *proc_count)
{
    if(args[1]==NULL||args[2]!=NULL)
    {
        return;
    }
    int pid=strtol(args[1],(char **)NULL,10),ii=0;
    for (int i = 0; i < 100; ++i)
    {
        if(proc_status[i]!=0)
        {
            ii++;
            if(pid==ii)
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
    kill(pid,21);
    kill(pid,18);
}
void execute_cmd(char *command,char *tilda,int proc_status[] , int *proc_count,char proc_name[][100],int *proc_id)
{
    char *cmd_first=strtok(command," \t \n");
    int space_flag=0;
    for (int i = 0; i < strlen(command)-1; ++i)
    {
        if(command[i]!=' ')
            space_flag=1;
    }
    if(space_flag==0)
        return;
    if(strcmp(command,"\n")==0)
        return ;
    /* printf("cmd_first = %s\n", cmd_first); */
    char *arg[10];
    arg[0]=cmd_first;
    int j,bg=0;
    for (j= 1; arg[j-1] != NULL ; j++)
    {
        arg[j]=strtok(NULL," \t \n");
    }
    j=j-1;
    if((j==0||j==1)&&arg[0][strlen(arg[0])-1]=='&')
    {
        bg=1;
        if(strlen(arg[0])-1>1)
            arg[0][strlen(arg[0])-1]='\0';
        else
            arg[0]=NULL;
    }
    else if(j!=0&&j!=1&&arg[j-1][strlen(arg[j-1])-1]=='&')
    {
        bg=1;
        if(strlen(arg[j-1])>1)
            arg[j-1][strlen(arg[j-1])-1]='\0';
        else
            arg[j-1]=NULL;
    }
    if(bg==1)
    {
        pid_t child=fork();
        /* if(child>0) */
        /*     printf("[%d]\n",child); */
        bg=0;
        if(child==0)
        {
            setpgid(0,0);
            int fail_ret=execvp(cmd_first,arg);
            if(fail_ret==-1)
            {
                printf("%s:Command not found\n",cmd_first);
            }
            exit(0);
        }
        else if(child>0)
        {
            printf("[%d]\n",child);
            strcpy(proc_name[(*proc_count)],cmd_first);
            proc_status[(*proc_count)++]=child;
        }
    }
    else if(strcmp(cmd_first,"cd")==0)
    {

        char *cmd_second=arg[1];
        if(cmd_second==NULL||strcmp(cmd_second,"~")==0)
        {
            cmd_second=tilda;
        }
        int ret=chdir(cmd_second);
        if(ret==-1)
        {
            perror("cd");
        }

    }
    else if(strcmp(cmd_first,"fg")==0)
    {
        fg_id(cmd_first,arg,proc_status,proc_count);
    }
    else if(strcmp(cmd_first,"bg")==0)
    {
        bg_id(cmd_first,arg,proc_status,proc_count);
    }
    else if(strcmp(cmd_first,"jobs")==0)
    {
        jobsppp(proc_status,proc_count,proc_name,proc_id);
    }
    else if(strcmp(cmd_first,"kjob")==0)
    {
        kjob(proc_status,arg);
    }
    else if(strcmp(cmd_first,"overkill")==0)
    {
        overkill(proc_status);
    }
    else if(cmd_first[0]=='\33'&&2<strlen(cmd_first)&&cmd_first[2]=='A')
    {
        up(cmd_first,tilda,proc_status,proc_count,proc_name,proc_id);
    }
    else if(strcmp(cmd_first,"setenv")==0)
    {
        set_env(cmd_first,arg);
    }
    else if(strcmp(cmd_first,"unsetenv")==0)
    {
        unset_env(cmd_first,arg);
    }
    else if(strcmp(cmd_first,"history")==0)
    {
        int len=10;
        if(arg[1]!=NULL)
            len=strtol(arg[1],(char **)NULL,10);
        p_hist(len,tilda);

    }
    else if(strcmp(cmd_first,"pinfo")==0)
    {
        pinfo(arg);
    }
    else if(strcmp(cmd_first,"ls")==0)
    {
        ls_init(arg);
    }
    else if(strcmp(cmd_first,"echo")==0)
    {
        for (int k = 1;arg[k]!=NULL ; ++k)
        {
            if(k==1&&arg[k][0]=='"')
            {
                if(arg[k+1]==NULL&&arg[k][strlen(arg[k])-1]=='"')
                {
                    arg[k][strlen(arg[k])-1]='\n';
                }
                printf("%s ",&arg[k][1]);
            }
            else if(arg[k+1]==NULL&&arg[k][strlen(arg[k])-1]=='"')
            {
                arg[k][strlen(arg[k])-1]='\n';
                printf("%s ",arg[k]);
            }
            else
                printf("%s ",arg[k]);
            }
        }
        else if(strcmp(cmd_first,"exit")==0||strcmp(cmd_first,"quit")==0)
        {
            overkill(proc_status);
            exit(0);
        }
        else if(strcmp(cmd_first,"pwd")==0)
        {
            char current_dir[350];
            getcwd(current_dir,350);
            printf("%s\n",current_dir);
        }
        else
        {
            pid_t child=fork();
            if(child==0)
            {
                int fail_ret=execvp(cmd_first,arg);
                if(fail_ret==-1)
                {
                    printf("%s:Command not found\n",cmd_first);
                }
                return 0;
            }
            if(child>0)
            {
                int status;
                 waitpid(child, &status, WUNTRACED);
                /* wait(0); */
            }
            if(fz)
            {
                (*proc_count)++;
                proc_status[*proc_count]=child;
                strcpy(proc_name[*proc_count],cmd_first);
                proc_id[*proc_count]=getpid();
                kill(getpid(),SIGTSTP);
                fz=0;
            }
            if(child==-1)
            {
                perror("");
            }
        }
}
