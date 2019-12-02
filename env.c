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

void set_env(char *cmd_first,char **args)
{
    if(args[1]==NULL||args[2]==NULL||args[3]!=NULL)
    {
        printf("Requires two arguments\n");
        return 0;
    }
    else
    {
        setenv(args[1],args[2],1);
    }
}
void unset_env(char *cmd_first,char **args)
{
    if(args[1]==NULL||args[2]!=NULL)
    {
        printf("Requires one arguments\n");
        return 0;
    }
    else
    {
        unsetenv(args[1]);
    }

}
