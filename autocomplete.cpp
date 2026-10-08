#include "shell.h"

int alreadyExists(char *name, char **matches, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(name, matches[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}

void autocomplete(char *input, int &pos)
{
    int start = pos;

    while (start > 0 &&
           input[start - 1] != ' ' &&
           input[start - 1] != '\t')
    {
        start--;
    }

    int isCommand = (start == 0);

    char prefix[MAX_INPUT];

    int len = pos - start;

    strncpy(prefix, input + start, len);

    prefix[len] = '\0';

    char *matches[MAX_INPUT];

    int count = 0;

    if (isCommand)
    {
        const char *builtins[] =
            {
                "cd",
                "echo",
                "pwd",
                "ls",
                "pinfo",
                "search"};

        int builtinCount = 6;

        for (int i = 0; i < builtinCount; i++)
        {
            if (strncmp(builtins[i], prefix, strlen(prefix)) == 0 &&
                !alreadyExists((char *)builtins[i], matches, count))
            {
                matches[count] = strdup(builtins[i]);

                count++;
            }
        }

        char *pathEnv = getenv("PATH");

        if (pathEnv != NULL)
        {
            char pathCopy[MAX_INPUT];

            strncpy(pathCopy, pathEnv, MAX_INPUT - 1);

            pathCopy[MAX_INPUT - 1] = '\0';

            char *save;

            char *dirPath = strtok_r(pathCopy, ":", &save);

            while (dirPath != NULL)
            {
                DIR *dir = opendir(dirPath);

                if (dir != NULL)
                {
                    struct dirent *entry;

                    while ((entry = readdir(dir)) != NULL)
                    {
                        if (strncmp(entry->d_name,
                                    prefix,
                                    strlen(prefix)) == 0)
                        {
                            char fullPath[PATH_MAX];

                            snprintf(fullPath,
                                    sizeof(fullPath),
                                    "%s/%s",
                                    dirPath,
                                    entry->d_name);

                            if (access(fullPath, X_OK) == 0 &&
                                !alreadyExists(entry->d_name, matches, count))
                            {
                                matches[count] = strdup(entry->d_name);

                                count++;
                            }
                        }
                    }

                    closedir(dir);
                }

                dirPath = strtok_r(NULL, ":", &save);
            }
        }
    }
    else
    {
        DIR *dir = opendir(".");

        if (dir == NULL)
        {
            return;
        }

        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL)
        {
            if (strncmp(entry->d_name, prefix, strlen(prefix)) == 0)
            {
                matches[count] = strdup(entry->d_name);

                count++;
            }
        }

        closedir(dir);
    }

    if (count == 0)
    {
        return;
    }

    if (count == 1)
    {
        char *match = matches[0];

        int matchlen = strlen(match);

        int remaining = matchlen - len;

        strcpy(input + pos, match + len);

        pos += remaining;

        printf("%s", match + len);

        fflush(stdout);

        free(matches[0]);

        return;
    }

    int commonLen = strlen(matches[0]);

    for (int i = 1; i < count; i++)
    {
        int j = 0;

        while (j < commonLen &&
               matches[0][j] != '\0' &&
               matches[i][j] != '\0' &&
               matches[0][j] == matches[i][j])
        {
            j++;
        }

        commonLen = j;
    }

    if (commonLen > len)
    {
        int remaining = commonLen - len;

        strncpy(input + pos, matches[0] + len, remaining);

        pos += remaining;

        input[pos] = '\0';

        printf("%.*s", remaining, matches[0] + len);

        fflush(stdout);
    }
    else
    {
        printf("\n");

        for (int i = 0; i < count; i++)
        {
            printf("%s", matches[i]);

            if (i != count - 1)
            {
                printf("  ");
            }
        }

        printf("\n");

        printPrompt();

        printf("%s", input);

        fflush(stdout);
    }

    for (int i = 0; i < count; i++)
    {
        free(matches[i]);
    }
}