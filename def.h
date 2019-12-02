#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdbool.h>
#include"redir.c"
#include"exec.c"
#include"pipe.c"
#include"env.c"

struct sys_data
{
    char hostname[250],username[250];
};
struct Proc
{
    int proc_status;
    char name[100];
    int status;
};

void status_bg(int *proc_status,int proc_count,char proc_name[][100]);

int get_args(int **arg);

char **get_name(int **arg,char names[][100]);

void print_perms(int args_int,char name[50]);

void ls_init(int **arg);

char *nth_word(char *proc_path,int n,char *ret);

char p_stat(char *proc_path);

void pinfo(char **arg);

int redir(char *cmd);

int check_redirect(char *cmd);

void execute_cmd(char *command,char *tilda,int proc_status[],int* proc_count,char proc_name[][100],int *proc_id);

int pipe_run(char *cmd,char *tilda,char *proc_status,int *proc_count,char proc_name[][100],int *proc_id);

int pipe_detect(char *cmd);

void jobsppp(int *proc_status,int *proc_count,char proc_name[][100],int *proc_id);

void set_env(char *cmd_first,char **args);

void unset_env(char *cmd_first,char **args);

void kjob(int proc_status[],char **arg);

void overkill(int proc_status[]);

void get_uh_names(struct sys_data *s)
{
    gethostname(s->hostname,250);
    getlogin_r(s->username,250);
}

bool tilda_beg(char *a,char *b)
{
    if(strncmp(a,b,strlen(b))==0)
        return 1;
    return 0;
}
char *current_d(char *dir,char *tilda)
{
    char current_dir[550];
    getcwd(current_dir,550);
    /* printf("cr:%s\n",current_dir); */
    if(tilda_beg(current_dir,tilda))
    {
        strcat(dir,&current_dir[strlen(tilda)]);
        /* printf("tilda:%s\n",dir); */
        return dir;
    }
    strcpy(dir,current_dir);
    return dir;
}
char *gen_prompt(char *strx,char *tilda)
{
    struct sys_data uhname;
    get_uh_names(&uhname);
    char dir[500]="~";
    current_d(dir,tilda);
    sprintf(strx,"%s@%s: \033[22;34m%s\033[0m\n\x1b[32m>\033[0m ",uhname.username,uhname.hostname,dir);
    return strx;
}

void tilda_init(char *tilda)
{
    for (int i = strlen(tilda)-1; i >= 0; --i)
    {
        if(tilda[i]=='/')
        {

            tilda[i]='\0';
            return;
        }
    }
}
void hist_init(char *command,char *tilda)
{
    if(command[0]=='\33')
        return;
    FILE *hist,*hist_temp;
    char *buff,path[1000];
    size_t buff_len=0;
    strcpy(path,tilda);
    strcat(path,"/temp");
    hist_temp=fopen(path,"w");
    strcpy(path,tilda);
    strcat(path,"/.hist");
    hist=fopen(path,"a+");
    fprintf(hist_temp, "%s",command);
    int i=19;
    while(getline(&buff,&buff_len,hist)>1&&i--!=0)
    {
        /* printf("i%d,%s\n",i,buff); */
        fprintf(hist_temp,"%s",buff);
    }
    fclose(hist);
    fclose(hist_temp);
    rename("temp",".hist");
}
void p_hist(int n,char *tilda)
{
    FILE *hist;
    if(n>10)
        n=10;
    char path[1000];
    strcpy(path,tilda);
    strcat(path,"/.hist");
    hist=fopen(path,"a+");
    char *buff;
    size_t buff_len=0;
    int count=0;
    char history[10][1000];
    while(getline(&buff,&buff_len,hist)>1&&n--!=0)
    {
        strcpy(history[count++],buff);
        /* printf("%s",buff); */
    }
    for (int i = 0; i < count; ++i)
    {
        printf("%s",history[count-1-i]);
    }
}
