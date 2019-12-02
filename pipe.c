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


int pipe_detect(char *cmd)
{
    int count=0;
    for (int i = 0; i < strlen(cmd); ++i)
    {
        if(cmd[i]=='|'&&i!=0&&cmd[i-1]=='|')
            cmd[i-1]=' ';
        else if(cmd[i]=='|')
            count++;
    }
    return count;

}
int pipe_run(char *cmd,char *tilda,char *proc_status,int *proc_count,char proc_name[][100],int * proc_id)
{
    int pipeno=pipe_detect(cmd);
    if(pipeno==0)
    {
        //nopipes
        return 0;
    }
    else
    {
        char *coms[100],*c,command[230];
        c=strtok(cmd,"|");
        coms[0]=c;
        for (int i = 1; coms[i-1]!=NULL; ++i)
        {
            coms[i]=strtok(NULL,"|");
        }
        int tmp=0;
        for (int i = 0; i < strlen(coms[pipeno])&&coms[pipeno]!=NULL; ++i)
        {
            if(coms[pipeno][i]!=' '&&coms[pipeno][i]!='\n')
                tmp=1;
        }
        if(coms[pipeno]==NULL||tmp==0)
        {
            printf("Pipe error\n\n");
            return 1;
        }
        for (int i = 0; i <= pipeno; ++i)
        {
            if(i==0)
            {
                int file_num=open("/tmp/shelltmpfile.tmp",O_WRONLY|O_CREAT|O_TRUNC,0644);
                int stdoutdup=dup(1);
                dup2(file_num,1);

                strcpy(command,coms[i]);
                int redir_check=redir(command);
                if(redir_check<0||redir_check==1)
                    continue;
                execute_cmd(command,tilda,proc_status,proc_count,proc_name,proc_id);

                dup2(stdoutdup,1);
                close(file_num);
            }
            else if(i==pipeno)
            {
                int file_num=open("/tmp/shelltmpfile.tmp",O_RDONLY);
                if(file_num==-1)
                {
                    printf("Error reading from file\n");
                    return 1;
                }
                int stdoutdup=dup(0);
                dup2(file_num,0);

                strcpy(command,coms[i]);
                int redir_check=redir(command);
                if(redir_check<0||redir_check==1)
                    continue;
                execute_cmd(command,tilda,proc_status,proc_count,proc_name,proc_id);

                dup2(stdoutdup,0);
                close(file_num);

            }
            else
            {
                int file_num_in=open("/tmp/shelltmpfile.tmp",O_RDONLY);
                int file_num_out=open("/tmp/shelltmpfile2.tmp",O_WRONLY|O_CREAT|O_TRUNC,0644);
                if(file_num_out==-1||file_num_in==-1)
                {
                    printf("Error reading from file\n");
                    return 1;
                }
                int stdoutdup_in=dup(0);
                int stdoutdup_out=dup(1);
                dup2(file_num_in,0);
                dup2(file_num_out,1);

                strcpy(command,coms[i]);
                int redir_check=redir(command);
                if(redir_check<0||redir_check==1)
                    continue;
                execute_cmd(command,tilda,proc_status,proc_count,proc_name,proc_id);

                dup2(stdoutdup_in,0);
                dup2(stdoutdup_out,1);
                close(file_num_in);
                close(file_num_out);
                rename("/tmp/shelltmpfile2.tmp","/tmp/shelltmpfile.tmp");
            }
        }
    }
}
