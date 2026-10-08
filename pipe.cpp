#include "shell.h"

void runPipe(char *cmd)
{
    char *parts[MAX_INPUT];

    int count = 0;

    char *save;

    char *part = strtok_r(cmd, "|", &save);

    while (part != NULL)
    {
        while (*part == ' ' || *part == '\t')
        {
            part++;
        }

        char *end = part + strlen(part) - 1;

        while (end >= part && (*end == ' ' || *end == '\t'))
        {
            *end = '\0';
            end--;
        }

        if (strlen(part) > 0)
        {
            parts[count] = part;
            count++;
        }

        part = strtok_r(NULL, "|", &save);
    }

    int prevpipe[2] = {-1, -1};
    pid_t pgid = -1;

    for (int i = 0; i < count; i++)
    {
        int curpipe[2] = {-1, -1};

        if (i < count - 1)
        {
            if (pipe(curpipe) == -1)
            {
                perror("pipe");
                return;
            }
        }

        pid_t pid = fork();

        if (pid == -1)
        {
            perror("fork");
            return;
        }

        if (pid == 0)
        {
            if (pgid == -1)
            {
                setpgid(0, 0);
            }
            else
            {
                setpgid(0, pgid);
            }

            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            signal(SIGTTOU, SIG_DFL);
            signal(SIGTTIN, SIG_DFL);

            char *args[MAX_INPUT];

            int argc = 0;

            char *infile = NULL;
            char *outfile = NULL;

            int append = 0;

            char *save2;

            char *arg = strtok_r(parts[i], " \t", &save2);

            while (arg != NULL)
            {
                if (strcmp(arg, "<") == 0)
                {
                    arg = strtok_r(NULL, " \t", &save2);

                    if (arg == NULL)
                    {
                        fprintf(stderr, "Invalid input redirection\n");
                        exit(EXIT_FAILURE);
                    }

                    infile = arg;
                }
                else if (strcmp(arg, ">") == 0)
                {
                    arg = strtok_r(NULL, " \t", &save2);

                    if (arg == NULL)
                    {
                        fprintf(stderr, "Invalid output redirection\n");
                        exit(EXIT_FAILURE);
                    }

                    outfile = arg;
                    append = 0;
                }
                else if (strcmp(arg, ">>") == 0)
                {
                    arg = strtok_r(NULL, " \t", &save2);

                    if (arg == NULL)
                    {
                        fprintf(stderr, "Invalid output redirection\n");
                        exit(EXIT_FAILURE);
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

            args[argc] = NULL;

            if (argc == 0)
            {
                exit(EXIT_SUCCESS);
            }

            if (i > 0)
            {
                if (dup2(prevpipe[0], STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            if (i < count - 1)
            {
                if (dup2(curpipe[1], STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            if (infile != NULL)
            {
                if (setupInput(infile) == -1)
                {
                    exit(EXIT_FAILURE);
                }
            }

            if (outfile != NULL)
            {
                if (setupOutput(outfile, append) == -1)
                {
                    exit(EXIT_FAILURE);
                }
            }

            if (prevpipe[0] != -1)
            {
                close(prevpipe[0]);
            }

            if (prevpipe[1] != -1)
            {
                close(prevpipe[1]);
            }

            if (curpipe[0] != -1)
            {
                close(curpipe[0]);
            }

            if (curpipe[1] != -1)
            {
                close(curpipe[1]);
            }

            runCmd(args, argc);

            fflush(stdout);

            exit(EXIT_SUCCESS);
        }

        if (pgid == -1)
        {
            pgid = pid;
        }

        setpgid(pid, pgid);

        if (prevpipe[0] != -1)
        {
            close(prevpipe[0]);
        }

        if (prevpipe[1] != -1)
        {
            close(prevpipe[1]);
        }

        prevpipe[0] = curpipe[0];
        prevpipe[1] = curpipe[1];
    }

    if (prevpipe[0] != -1)
    {
        close(prevpipe[0]);
    }

    if (prevpipe[1] != -1)
    {
        close(prevpipe[1]);
    }

    pid_t shellPgid = getpgrp();

    foregroundPid = pgid;

    tcsetpgrp(STDIN_FILENO, pgid);

    for (int i = 0; i < count; i++)
    {
        waitpid(-pgid, NULL, WUNTRACED);
    }

    tcsetpgrp(STDIN_FILENO, shellPgid);

    foregroundPid = -1;
}