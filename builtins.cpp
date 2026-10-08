#include "shell.h"

void runPwd()
{
    char cwd[PATH_MAX];

    if (getcwd(cwd, PATH_MAX) == NULL)
    {
        perror("pwd");
        return;
    }

    printf("%s\n", cwd);
}

void runEcho(char **args, int argc)

{
    for (int i = 1; i < argc; i++)
    {
        printf("%s", args[i]);

        if (i != argc - 1)
        {
            printf(" ");
        }
    }

    printf("\n");
}

void runCd(char **args, int argc)
{
    if (argc > 2)
    {
        printf("Invalid arguments for error handling\n");
        return;
    }

    char cwd[PATH_MAX];

    if (getcwd(cwd, PATH_MAX) == NULL)
    {
        perror("cd");
        return;
    }

    if (argc == 1 || strcmp(args[1], "~") == 0)
    {
        if (chdir(homedir) == -1)
        {
            perror("cd");
            return;
        }

        strcpy(prevdir, cwd);
        return;
    }

    if (strcmp(args[1], "-") == 0)
    {
        if (chdir(prevdir) == -1)
        {
            perror("cd");
            return;
        }

        strcpy(prevdir, cwd);
        return;
    }

    if (chdir(args[1]) == -1)
    {
        perror("cd");
        return;
    }

    strcpy(prevdir, cwd);
}

void runLs(char **args, int argc)
{
    int showall = 0;
    int longlist = 0;
    int dircount = 0;

    if (argc == 1)
    {
        listDir((char *)".", showall, longlist);
        return;
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(args[i], "-a") == 0)
        {
            showall = 1;
        }
        else if (strcmp(args[i], "-l") == 0)
        {
            longlist = 1;
        }
        else if (strcmp(args[i], "-la") == 0 ||
                 strcmp(args[i], "-al") == 0)
        {
            showall = 1;
            longlist = 1;
        }
        else
        {
            dircount++;
        }
    }

    if (dircount == 0)
    {
        listDir((char *)".", showall, longlist);
        return;
    }

    for (int i = 1; i < argc; i++)
    {
        if (args[i][0] == '-')
        {
            continue;
        }

        listDir(args[i], showall, longlist);
    }
}

void listDir(char *path, int showall, int longlist)
{
    if (strcmp(path, "~") == 0)
    {
        path = homedir;
    }

    DIR *dir = opendir(path);

    if (dir == NULL)
    {
        perror(path);
        return;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (!showall && entry->d_name[0] == '.')
        {
            continue;
        }

        if (longlist)
        {
            char fullpath[PATH_MAX];

            snprintf(fullpath, PATH_MAX, "%s/%s",
                     path, entry->d_name);

            listLong(fullpath, entry->d_name);
        }
        else
        {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
}

void listLong(char *path, char *name)
{
    struct stat st;

    if (stat(path, &st) == -1)
    {
        perror(name);
        return;
    }

    char perm[11];

    if (S_ISDIR(st.st_mode))
    {
        perm[0] = 'd';
    }
    else
    {
        perm[0] = '-';
    }

    perm[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
    perm[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
    perm[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';

    perm[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
    perm[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
    perm[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';

    perm[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
    perm[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
    perm[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';

    perm[10] = '\0';

    struct passwd *pw = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);

    char *username = pw ? pw->pw_name : (char *)"unknown";
    char *groupname = gr ? gr->gr_name : (char *)"unknown";

    char timebuf[64];

    struct tm *tm = localtime(&st.st_mtime);

    if (tm != NULL)
    {
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm);
    }
    else
    {
        strcpy(timebuf, "unknown");
    }

    printf("%s %ld %s %s %ld %s %s\n",
           perm,
           (long)st.st_nlink,
           username,
           groupname,
           (long)st.st_size,
           timebuf,
           name);
}

void runPinfo(char **args, int argc)

{

    if (argc > 2)

    {

        printf("Invalid arguments\n");

        return;

    }

    pid_t pid;

    if (argc == 1)

    {

        pid = getpid();

    }

    else

    {

        char *end;

        long num = strtol(args[1], &end, 10);

        if (*args[1] == '\0' || *end != '\0' || num <= 0)

        {

            printf("Invalid PID\n");

            return;

        }

        pid = (pid_t)num;

    }

    char path[PATH_MAX];

    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    FILE *file = fopen(path, "r");

    if (file == NULL)

    {

        perror("pinfo");

        return;

    }

    char line[1024];

    char status = '\0';

    char memory[256];

    memory[0] = '\0';

    while (fgets(line, sizeof(line), file) != NULL)

    {

        if (strncmp(line, "State:", 6) == 0)

        {

            sscanf(line, "State:\t%c", &status);

        }

        else if (strncmp(line, "VmSize:", 7) == 0)

        {

            char value[128];

            if (sscanf(line, "VmSize:%127s", value) == 1)

            {

                strcpy(memory, value);

            }

        }

    }

    fclose(file);

    if (status == '\0')

    {

        printf("Could not get process information\n");

        return;

    }

    int foreground = 0;

    if (pid == getpid())

    {

        foreground = 1;

    }

    snprintf(path, sizeof(path), "/proc/%d/exe", pid);

    char exepath[PATH_MAX];

    ssize_t len = readlink(path, exepath, sizeof(exepath) - 1);

    if (len == -1)

    {

        perror("pinfo");

        return;

    }

    exepath[len] = '\0';

    int homedirlen = strlen(homedir);

    char shownpath[PATH_MAX];

    if (strncmp(exepath, homedir, homedirlen) == 0 &&

        (exepath[homedirlen] == '/' || exepath[homedirlen] == '\0'))

    {

        snprintf(shownpath, sizeof(shownpath), "~%s",

                 exepath + homedirlen);

    }

    else

    {

        strcpy(shownpath, exepath);

    }

    printf("Process Status -- %c", status);

    if (foreground)

    {

        printf("+");

    }

    printf("\n");

    printf("memory -- %s {Virtual Memory}\n", memory);

    printf("Executable Path -- %s\n", shownpath);

}
