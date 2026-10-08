#include "shell.h"

void initShell()
{
    // get current working directory
    if (getcwd(homedir, PATH_MAX) == NULL)
    {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    strcpy(prevdir, homedir);
}

void printPrompt()
{
    struct passwd *pw = getpwuid(getuid());

    char username[256];

    if (pw == NULL)
    {
        strcpy(username, "unknown");
    }
    else
    {
        strcpy(username, pw->pw_name);
    }

    char sysname[256];

    if (gethostname(sysname, sizeof(sysname)) == -1)
    {
        perror("gethostname");
        strcpy(sysname, "unknown");
    }

    char cwd[PATH_MAX];

    if (getcwd(cwd, PATH_MAX) == NULL)
    {
        perror("getcwd");
        return;
    }

    int homedirlen = strlen(homedir);

    if (strcmp(cwd, homedir) == 0)
    {
        printf("<%s@%s:~> ", username, sysname);
    }

    else if (strncmp(cwd, homedir, homedirlen) == 0 &&
             cwd[homedirlen] == '/')
    {
        printf("<%s@%s:~%s> ", username, sysname, cwd + homedirlen);
    }
    else
    {
        printf("<%s@%s:%s> ", username, sysname, cwd);
    }

    fflush(stdout);
}

void runSearch(char **args, int argc)

{

    if (argc != 2)

    {

        printf("Invalid arguments\n");

        return;
    }

    char cwd[PATH_MAX];

    if (getcwd(cwd, PATH_MAX) == NULL)

    {

        perror("getcwd");

        return;
    }

    if (searchFile(cwd, args[1]))

    {

        printf("True\n");
    }

    else

    {

        printf("False\n");
    }
}

int searchFile(char *path, char *name)

{

    DIR *dir = opendir(path);

    if (dir == NULL)

    {

        return 0;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)

    {

        if (strcmp(entry->d_name, ".") == 0 ||

            strcmp(entry->d_name, "..") == 0)

        {

            continue;
        }

        if (strcmp(entry->d_name, name) == 0)

        {

            closedir(dir);

            return 1;
        }

        char newpath[PATH_MAX];

        snprintf(newpath, PATH_MAX, "%s/%s",

                 path, entry->d_name);

        struct stat info;

        if (stat(newpath, &info) == -1)

        {

            continue;
        }

        if (S_ISDIR(info.st_mode))

        {

            if (searchFile(newpath, name))

            {

                closedir(dir);

                return 1;
            }
        }
    }

    closedir(dir);

    return 0;
}