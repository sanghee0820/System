#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

typedef struct Files
{
    char mode[11];
    int st_nlink;
    char *uid, *gid, *filename;;
    long size;
    time_t time;
    struct Files *next;
} Dir_Files;

void do_ls(char[], char);
Dir_Files *dostat(char *);
void show_stat_info(char *, struct stat *);
Dir_Files *show_file_info(char *, struct stat *);
void mode_to_letters(int, char[]);
void printData(Dir_Files *);
char *uid_to_name(uid_t);
char *gid_to_name(gid_t);
char currentDir[50];

int main(int argc, char *argv[])
{
    char type = 0; // standard
    getcwd(currentDir, 50);

    if (argc == 1)
        do_ls(".", type);
    // Time
    else if (!strcmp(argv[1], "-t"))
    {
        type = 1;
        argc--;
        argv++;
        if (argc > 1)
            while (--argc)
            {
                printf("%s\n", *++argv);
                do_ls(*argv, type);
            }
        else
            do_ls(".", type);
    }
    // Size
    else if (!strcmp(argv[1], "-b"))
    {
        type = 2;
        argc--;
        ++argv;
        if (argc > 1)
            while (--argc)
            {
                printf("%s\n", *++argv);
                do_ls(*argv, type);
            }
        else
            do_ls(".", type);
    }
    else if (argv[1][0] == '-')
    {
        fprintf(stderr, "invalid <type>, Please Type <-t> or <-b>\n");
        exit(1);
    }
    else
        while (--argc)
        {
            printf("%s\n", *++argv);
            do_ls(*argv, type);
        }

    return 0;
}

void do_ls(char dirname[], char type)
{
    DIR *dir_ptr;
    struct dirent *direntp;

    if ((dir_ptr = opendir(dirname)) == NULL)
        fprintf(stderr, "ls1: cannot open %s\n", dirname);
    else
    {
        chdir(dirname);
        Dir_Files *root = NULL, *tail = NULL, *temp;

        while ((direntp = readdir(dir_ptr)) != NULL)
        {
            Dir_Files *newdir = dostat(direntp->d_name);

            if (!root)
                root = tail = newdir;
            else
            {
                if (type == 0) // standard
                {
                    tail->next = newdir;
                    tail = newdir;
                }
                //Sorting
                else if (type == 1)  // Time
                {
                    if (root->time > newdir->time)
                    {
                        newdir->next = root;
                        root = newdir;
                        continue;
                    }
                    for (temp = root; temp->next; temp = temp->next)
                    {
                        if (temp->time <= newdir->time && newdir->time < temp->next->time)
                        {
                            newdir->next = temp->next;
                            temp->next = newdir;
                            break;
                        }
                    }
                    if (temp == tail){
                        tail->next = newdir;
                        tail = newdir;
                    }
                }
                else if (type == 2)  // Size
                {
                    if (root->size < newdir->size)
                    {
                        newdir->next = root;
                        root = newdir;
                        continue;
                    }
                    for (temp = root; temp->next; temp = temp->next)
                    {
                        if (temp->size >= newdir->size && newdir->size > temp->next->size)
                        {
                            newdir->next = temp->next;
                            temp->next = newdir;
                            break;
                        }
                    }
                    if (temp == tail){
                        tail->next = newdir;
                        tail = newdir;
                    }
                }
                else
                    fprintf(stderr, "Error\n");
            }
        }
        printData(root);
        closedir(dir_ptr);
        chdir(currentDir);
    }
}

Dir_Files *dostat(char *filename)
{
    struct stat info;
    if (stat(filename, &info) == -1)
    {
        perror(filename);
        return NULL;
    }
    else
        return show_file_info(filename, &info);
}

Dir_Files *show_file_info(char *filename, struct stat *info_p)
{
    char *uid_to_name(), *ctime(), *gid_to_name(), *filemode();
    void mode_to_letters();
    char modestr[11];

    Dir_Files *new = (Dir_Files *)malloc(sizeof(Dir_Files));
    if (!new)
    {
        fprintf(stderr, "\n");
        exit(1);
    }
    mode_to_letters(info_p->st_mode, modestr);

    strcpy(new->mode, modestr);
    new->st_nlink = (int)info_p->st_nlink;
    new->uid = uid_to_name(info_p->st_uid);
    new->gid = gid_to_name(info_p->st_gid);
    new->size = (long)info_p->st_size;
    new->time = info_p->st_mtime;
    new->filename = filename;
    new->next = NULL;

    return new;
}

void printData(Dir_Files *root)
{
    char *ctime();

    int count = 0;
    long size_sum = 0;
    for (Dir_Files *temp = root; temp; temp = temp->next)
    {
        count++;
        size_sum += temp->size;
        printf("%s", temp->mode);
        printf("%4d ", temp->st_nlink);
        printf("%-8s ", temp->uid);
        printf("%-8s ", temp->gid);
        printf("%-8ld ", temp->size);
        printf("%.12s ", 4 + ctime(&temp->time));
        printf("%s \n", temp->filename);
    }
    printf("합계 %d, 총 %ld Bytes \n", count, size_sum);
}

void mode_to_letters(int mode, char str[])
{
    strcpy(str, "----------");

    if (S_ISDIR(mode))
        str[0] = 'd';
    if (S_ISCHR(mode))
        str[0] = 'c';
    if (S_ISBLK(mode))
        str[0] = 'b';

    if (mode & S_IRUSR)
        str[1] = 'r';
    if (mode & S_IWUSR)
        str[2] = 'w';
    if (mode & S_IXUSR)
        str[3] = 'x';

    if (mode & S_IRGRP)
        str[4] = 'r';
    if (mode & S_IWGRP)
        str[5] = 'w';
    if (mode & S_IXGRP)
        str[6] = 'x';

    if (mode & S_IROTH)
        str[7] = 'r';
    if (mode & S_IWOTH)
        str[8] = 'w';
    if (mode & S_IXOTH)
        str[9] = 'x';
}

char *uid_to_name(uid_t uid)
{
    struct passwd *getpwuid(), *pw_ptr;
    static char numstr[10];

    if ((pw_ptr = getpwuid(uid)) == NULL)
    {
        sprintf(numstr, "%d", uid);
        return numstr;
    }
    else
        return pw_ptr->pw_name;
}

char *gid_to_name(gid_t gid)
{
    struct group *getgrid(), *grp_ptr;
    static char numstr[10];
    if ((grp_ptr = getgrgid(gid)) == NULL)
    {
        sprintf(numstr, "%d", gid);
        return numstr;
    }
    return grp_ptr->gr_name;
}
