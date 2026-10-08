#include "shell.h"

void parseInput(char *input)
{
    //for formatting the input like adding spaces and all
    char formatted[MAX_INPUT * 3];
    int j = 0;

    for (int i = 0; input[i] != '\0'; i++)
    {
        if (input[i] == '>')
        {
            formatted[j++] = ' ';

            if (input[i + 1] == '>')
            {
                formatted[j++] = '>';
                formatted[j++] = '>';
                i++;//moves past second >
            }
            else
            {
                formatted[j++] = '>';
            }

            formatted[j++] = ' ';
        }
        else if (input[i] == '<' ||
                 input[i] == '|' ||
                 input[i] == '&' ||
                 input[i] == ';')
        {
            formatted[j++] = ' ';
            formatted[j++] = input[i];
            formatted[j++] = ' ';
        }
        else
        {
            formatted[j++] = input[i];
        }
    }

    formatted[j] = '\0';

    char *save1;

    char *cmd = strtok_r(formatted, ";", &save1);

    while (cmd != NULL)
    {
        parseCmd(cmd);

        cmd = strtok_r(NULL, ";", &save1);
    }
}

void parseCmd(char *cmd)

{
    if (strchr(cmd, '|') != NULL)
    {
        runPipe(cmd);
        return;
    }

    char *args[MAX_INPUT];

    int argc = 0;

    char *infile = NULL;

    char *outfile = NULL;

    int append = 0;

    char *save2;

    char *arg = strtok_r(cmd, " \t", &save2);

    while (arg != NULL)

    {

        if (strcmp(arg, "<") == 0)

        {

            arg = strtok_r(NULL, " \t", &save2);

            if (arg == NULL)

            {

                printf("Invalid input redirection\n");

                return;
            }

            infile = arg;
        }

        else if (strcmp(arg, ">") == 0)

        {

            arg = strtok_r(NULL, " \t", &save2);

            if (arg == NULL)

            {

                printf("Invalid output redirection\n");

                return;
            }

            outfile = arg;

            append = 0;
        }

        else if (strcmp(arg, ">>") == 0)

        {

            arg = strtok_r(NULL, " \t", &save2);

            if (arg == NULL)

            {

                printf("Invalid output redirection\n");

                return;
            }

            outfile = arg;

            append = 1;
        }

        else

        {

            args[argc] = arg;

            argc++;
        }

        arg = strtok_r(NULL, " \t", &save2);
    }

    if (argc == 0)

    {

        return;
    }

    args[argc] = NULL;

    runRedirect(args, argc, infile, outfile, append);
}

int setupInput(char *file)

{

    int fd = open(file, O_RDONLY);

    if (fd == -1)

    {

        perror(file);

        return -1;

    }

    if (dup2(fd, STDIN_FILENO) == -1)

    {

        perror("dup2");

        close(fd);

        return -1;

    }

    close(fd);

    return 0;

}

int setupOutput(char *file, int append)

{

    int fd;

    if (append)

    {

        fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);

    }

    else

    {

        fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    }

    if (fd == -1)

    {

        perror(file);

        return -1;

    }

    if (dup2(fd, STDOUT_FILENO) == -1)

    {

        perror("dup2");

        close(fd);

        return -1;

    }

    close(fd);

    return 0;

}

void runRedirect(char **args, int argc, char *infile, char *outfile, int append)

{

    int oldin = -1;

    int oldout = -1;

    if (infile != NULL)

    {

        oldin = dup(STDIN_FILENO);

        if (oldin == -1)

        {

            perror("dup");

            return;

        }

        if (setupInput(infile) == -1)

        {

            close(oldin);

            return;

        }

    }

    if (outfile != NULL)

    {

        oldout = dup(STDOUT_FILENO);

        if (oldout == -1)

        {

            perror("dup");

            if (oldin != -1)

            {

                dup2(oldin, STDIN_FILENO);

                close(oldin);

            }

            return;

        }

        if (setupOutput(outfile, append) == -1)

        {

            close(oldout);

            if (oldin != -1)

            {

                dup2(oldin, STDIN_FILENO);

                close(oldin);

            }

            return;

        }

    }

    runCmd(args, argc);

    if (oldin != -1)

    {

        dup2(oldin, STDIN_FILENO);

        close(oldin);

    }

    if (oldout != -1)

    {

        dup2(oldout, STDOUT_FILENO);

        close(oldout);

    }

}