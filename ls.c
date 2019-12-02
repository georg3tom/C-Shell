#include<stdio.h>
#include<dirent.h>
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

int get_args(int **arg)
{
    int x=0;
    char word1[100];
    for (int i=0; arg[i]!=NULL; i++)
    {
        strcpy(word1,arg[i]);
        /* printf("%s,%c\n",arg[i],word1[0]); */
        if(word1[0]=='-')
        {
            for (int j = 1; j < strlen(word1); ++j)
            {
                /* printf("%c,%d\n",word1[j],word1[j]=='l'); */
                if(word1[j]=='l')
                {
                    x=x|1;
                }
                else if(word1[j]=='a')
                {
                    x=x|2;
                }
            }
        }
    }
    return x;
}

char **get_name(int **arg,char names[][100])
{
    int x=0;
    char word1[100];
    strcpy(names[0],"");
    for (int i=1,k=0;arg[i]!=NULL; i++)
    {
        strcpy(word1,arg[i]);
        /* printf("%s,%c\n",arg[i],word1[0]); */
        if(word1[0]!='-')
        {
            strcpy(names[k++],word1);
            strcpy(names[k],"");
        }
    }
        return names;
}
void print_perms(int args_int,char name[50])
{
    if(args_int==0||args_int==2)
        return;
    struct stat check;
    stat(name,&check);
    char date[10];
    char perms[15]="";
    int file_flag=check.st_mode&S_IFMT;
    if(file_flag==S_IFREG)
        strcat(perms,"-");
    else if(file_flag==S_IFDIR)
        strcat(perms,"d");
    else if(file_flag==S_IFLNK)
        strcat(perms,"l");

    strcat(perms,((check.st_mode&S_IRUSR)?"r":"-"));
    strcat(perms,((check.st_mode&S_IWUSR)?"w":"-"));
    strcat(perms,((check.st_mode&S_IXUSR)?"x":"-"));
    strcat(perms,((check.st_mode&S_IRGRP)?"r":"-"));
    strcat(perms,((check.st_mode&S_IWGRP)?"w":"-"));
    strcat(perms,((check.st_mode&S_IXGRP)?"x":"-"));
    strcat(perms,((check.st_mode&S_IROTH)?"r":"-"));
    strcat(perms,((check.st_mode&S_IWOTH)?"w":"-"));
    strcat(perms,((check.st_mode&S_IXOTH)?"x ":"- "));
    printf("%s",perms);
    printf("%d ",check.st_nlink);
    printf("%s ",getpwuid(check.st_uid)->pw_name);
    printf("%s ",getgrgid(check.st_gid)->gr_name);
    printf("%8d ", (long long int)check.st_size);
    strftime(date, 20, "%d-%m-%y", localtime(&(check.st_ctime)));
    printf("%s ",date);

}
void ls_init(int **arg)
{
    int args_int=get_args(arg);
    char names[100][100];
    get_name(arg,names);
    struct stat check;
    if(strcmp(names[0],"")==0)
    {
        strcpy(names[0],".");
        strcpy(names[1],"");
    }
    for (int i = 0; strcmp(names[i],"")!=0; ++i)
    {
        if(stat(names[i],&check)==0)
        {
            if(S_ISDIR(check.st_mode))
            {
                DIR *f_dir;
                struct dirent *f_file;
                f_dir=opendir(names[i]);
                printf("%s:\n",names[i]);
                while((f_file=readdir(f_dir))!=NULL)
                {
                    char file_path[100];
                    strcpy(file_path,names[i]);
                    strcat(file_path,"/");
                    strcat(file_path,f_file->d_name);
                    if((args_int==2||args_int==3)&&f_file->d_name[0]=='.')
                    {
                        print_perms(args_int,file_path);
                        printf("%s  ",f_file->d_name);
                        if(args_int==1||args_int==3)
                            printf("\n");
                    }
                    else if(f_file->d_name[0]!='.')
                    {
                        print_perms(args_int,file_path);
                        printf("%s  ",f_file->d_name);
                        if(args_int==1||args_int==3)
                            printf("\n");
                    }
                }
            }
            else
            {
                if((args_int==2||args_int==3)&&names[i][0]=='.')
                {
                    print_perms(args_int,names[i]);
                    printf("%s\n",names[i]);
                }
                else if(names[i][0]!='.')
                {
                    print_perms(args_int,names[i]);
                    printf("%s\n",names[i]);
                }
                if(args_int==1||args_int==3)
                    printf("\n");
            }
        }
    }
    printf("\n");
}
