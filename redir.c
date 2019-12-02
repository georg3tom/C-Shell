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

int fileparse(char *outfile,char *c)
{
    int f=1;
    for (int i = 0; i < strlen(outfile); ++i)
    {
        if(outfile[i]!=' ')
        {
            f=0;
            break;
        }
    }
    if(f==1)
        return -2;
    for (int i = 0; i < strlen(outfile); ++i)
    {
        if(outfile[i]!=' ')
        {
            strcpy(c,&outfile[i]);
            break;
        }
    }
    for (int i = 0; i < strlen(c); ++i)
    {
        if(c[i]==' ')
        {
            c[i]='\0';
            break;
        }
    }
    return 1;
}
int check_redirect(char *cmd)
{
    int f=0,count=0;
    for (int i = 0; i < strlen(cmd); ++i)
    {
        if(cmd[i]=='<'||cmd[i]=='>')
        {
            if(cmd[i]=='<')
                f=1;
            else
                f=2;
            count++;
            if(count==2)
            {
                if(cmd[i]=='<'&&cmd[i-1]=='<')
                {
                    cmd[i]=' ';
                    count--;
                }
                else if(cmd[i]=='>'&&cmd[i-1]=='>')
                {
                    f=4;
                }
                else
                    f=-1;
            }
            
        }
    }
    int tmp=0;
    if(count==2||count==3)
    {
        for (int i = 0; i < strlen(cmd); ++i)
        {
            if(cmd[i]=='<'&&i>0&&cmd[i-1]!='>')
            {
                if(tmp==0)
                    tmp=1;
            }
            else if(cmd[i]=='>'&&i>0&&cmd[i-1]!='<')
            {
                if(tmp==1)
                    tmp=2;
            }
            else if(cmd[i]=='>'&&i>0&&cmd[i-1]!='>')
            {
                if(tmp==1)
                    tmp=3;
            }
        }

    }
    if(tmp==2||tmp==3)
    {
        f=5;
        if(tmp==3)
            f=6;

    }
    if(count>2&&tmp==0)
        count=-1;
    if(count<=0)
    {
        return count;
    }
    else
        return f;
}

int redir(char *cmd)
{
    int x=check_redirect(cmd);
    if(x<=0)
    {
        if(x==-1)
            printf("Error Parsing\n\n");
        return x;
    }
    if(x==5||x==6)
    {
        for (int i = 0; i < strlen(cmd); ++i)
        {
            if(cmd[i]=='>')
                cmd[i]='<';
            if(cmd[i]=='<'&&i<0&&cmd[i-1]=='<')
                cmd[i-1]=' ';
        }
        char *parts[4],*c,f1[100],f2[100];
        c=strtok(cmd,"<");
        parts[0]=c;
        parts[1]=strtok(NULL,"<");
        parts[2]=strtok(NULL,"<");
        int file_parse_status=fileparse(parts[1],f1);
        file_parse_status=fileparse(parts[2],f2);
        f2[strlen(f2)-1]='\0';
        int file_num_in=open(f1,O_RDONLY),file_num_out;
        if(x==6)
            file_num_out=open(f2,O_WRONLY|O_CREAT|O_APPEND,0644);
        else
            file_num_out=open(f2,O_WRONLY|O_CREAT|O_TRUNC,0644);
        if(file_num_out==-1||file_num_in==-1)
        {
            printf("Error reading from file\n");
        }
        int stdoutdup_in=dup(0);
        int stdoutdup_out=dup(1);
        dup2(file_num_in,0);
        dup2(file_num_out,1);

        execute_cmd(parts[0],NULL,NULL,NULL);

        dup2(stdoutdup_in,0);
        dup2(stdoutdup_out,1);
        close(file_num_in);
        close(file_num_out);

        
        /* int file_num=open(f2,O_WRONLY|O_TRUNC|O_CREAT,0644); */
        /* int file_num_std=open("/tmp/shelltmpfile2.tmp",O_RDONLY,0644); */
        /* int stdoutdup=dup(0); */
        /* int stdoutdup2=dup(1); */
        /* dup2(file_num,0); */
        /* dup2(file_num_std,1); */
        /* execute_cmd(parts[2],NULL,NULL,NULL); */
        /* dup2(stdoutdup,0); */
        /* dup2(stdoutdup2,1); */
        /* close(file_num); */
        /* close(file_num_std); */
    }
    else
    {
        if(x==2||x==4)
        {
            if(x==2)
            {
                char *parts[3],*c;
                c=strtok(cmd,">");
                parts[0]=c;
                parts[1]=strtok(NULL,">");
                char *filename[100],*file_args[100];
                parts[1][strlen(parts[1])-1]='\0';
                int file_parse_status=fileparse(parts[1],filename);
                if(fileparse<0)
                {
                    printf("Error parsing\n");
                    return 1;
                }
                int file_num=open(filename,O_WRONLY|O_TRUNC|O_CREAT,0644);
                int stdoutdup=dup(1);
                dup2(file_num,1);
                execute_cmd(parts[0],NULL,NULL,NULL);
                dup2(stdoutdup,1);
                close(file_num);
            }
            else

            {
                char *parts[3],*c;
                c=strtok(cmd,">>");
                parts[0]=c;
                parts[1]=strtok(NULL,">>");
                char *filename[100],*file_args[100];
                parts[1][strlen(parts[1])-1]='\0';
                int file_parse_status=fileparse(parts[1],filename);
                if(fileparse<0)
                {
                    printf("Error parsing\n");
                    return 1;
                }
                int file_num=open(filename,O_WRONLY|O_CREAT|O_APPEND,0644);
                int stdoutdup=dup(1);
                dup2(file_num,1);
                execute_cmd(parts[0],NULL,NULL,NULL);
                dup2(stdoutdup,1);
                close(file_num);
            }
        }
        else
        {
            if(x==1)
            {
                char *parts[3],*c;
                c=strtok(cmd,"<");
                parts[0]=c;
                parts[1]=strtok(NULL,"<");
                char *filename[100],*file_args[100];
                parts[1][strlen(parts[1])-1]='\0';
                int file_parse_status=fileparse(parts[1],filename);
                if(fileparse<0)
                {
                    printf("Error parsing\n");
                    return 1;
                }
                int file_num=open(filename,O_RDONLY);
                if(file_num==-1)
                {
                    printf("No such file or directory\n");
                    return -1;
                }
                int stdoutdup=dup(0);
                dup2(file_num,0);
                execute_cmd(parts[0],NULL,NULL,NULL);
                dup2(stdoutdup,0);
                close(file_num);
            }
            else
            {
                char *parts[3],*c;
                c=strtok(cmd,"<<");
                parts[0]=c;
                parts[1]=strtok(NULL,"<<");
                char *filename[100],*file_args[100];
                parts[1][strlen(parts[1])-1]='\0';
                int file_parse_status=fileparse(parts[1],filename);
                if(fileparse<0)
                {
                    printf("Error parsing\n");
                    return 1;
                }
                int file_num=open(filename,O_RDONLY);
                if(file_num==-1)
                {
                    printf("No such file or directory\n");
                    return -1;
                }
                int stdoutdup=dup(0);
                dup2(file_num,0);
                execute_cmd(parts[0],NULL,NULL,NULL);
                dup2(stdoutdup,0);
                close(file_num);
            }
        }
    }
    return 1;
}


