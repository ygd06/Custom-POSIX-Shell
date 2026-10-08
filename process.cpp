#include "shell.h"

void runSysCmd(char **args, int argc)
{
    int bg = 0;

    if (argc > 1 && strcmp(args[argc - 1], "&") == 0)
    {
        bg = 1;

        args[argc - 1] = NULL;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");

        return;
    }

    if (pid == 0)
    {
        setpgid(0, 0);

        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);

        execvp(args[0], args);

        perror(args[0]);

        exit(1);
    }
    else
    {
        setpgid(pid, pid);

        if (bg)
        {
            printf("%d\n", pid);
        }
        else
        {
            pid_t shellPgid = getpgrp();

            foregroundPid = pid;

            tcsetpgrp(STDIN_FILENO, pid);

            waitpid(pid, NULL, WUNTRACED);

            tcsetpgrp(STDIN_FILENO, shellPgid);

            foregroundPid = -1;
        }
    }
}

void runCmd(char **args, int argc)
{
    if (strcmp(args[0], "pwd") == 0)
    {
        runPwd();
        return;
    }

    if (strcmp(args[0], "echo") == 0)
    {
        runEcho(args, argc);
        return;
    }

    if (strcmp(args[0], "cd") == 0)
    {
        runCd(args, argc);
        return;
    }

    if (strcmp(args[0], "ls") == 0)
    {
        runLs(args, argc);
        return;
    }

    if (strcmp(args[0], "pinfo") == 0)

    {
        runPinfo(args, argc);
        return;
    }

    if (strcmp(args[0], "search") == 0)

    {
        runSearch(args, argc);
        return;
    }

    if (strcmp(args[0], "history") == 0)
    {
        runHistory(args, argc);
        return;
    }

    runSysCmd(args, argc);
}