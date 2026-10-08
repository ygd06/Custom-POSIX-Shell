#include "shell.h"

char homedir[PATH_MAX];
char prevdir[PATH_MAX];
volatile sig_atomic_t foregroundPid = -1;

void handleSigint(int sig)
{
    if (foregroundPid != -1)
    {
        
        kill(-foregroundPid, SIGINT);
    }
}

void handleSigtstp(int sig)
{
    if (foregroundPid != -1)
    {
        kill(-foregroundPid, SIGTSTP);
    }
}

void handleSigchld(int sig)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
    }
}

int main()
{
    initShell();

    loadHistory();
    
    signal(SIGINT, handleSigint);
    signal(SIGTSTP, handleSigtstp);
    signal(SIGCHLD, handleSigchld);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    pid_t shellPgid = getpid();
    setpgid(shellPgid, shellPgid);
    tcsetpgrp(STDIN_FILENO, shellPgid);

    
    char input[MAX_INPUT] = {0};

    while (true)
    {
        printPrompt();

        int pos = 0;

        
        int historyIndex = historyCount;

        
        struct termios oldt, newt;

        
        tcgetattr(STDIN_FILENO, &oldt);

        newt = oldt;

        newt.c_lflag &= ~(ICANON | ECHO);

        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        while (true)
        {
            char ch;

            
            if (read(STDIN_FILENO, &ch, 1) <= 0)
            {
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

                printf("\n");

                return 0;
            }

            if (ch == '\n')
            {
                printf("\n");

                input[pos] = '\0';

                break;
            }

            else if (ch == 127 || ch == '\b')
            {
                if (pos > 0)
                {
                    pos--;

                    input[pos] = '\0';

                    printf("\b \b");

                    fflush(stdout);
                }
            }

            else if (ch == '\t')
            {
                autocomplete(input, pos);
            }

            else if (ch == 4)
            {
                if (pos == 0)
                {
                    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

                    printf("\n");

                    return 0;
                }
            }

            
            else if (ch == '\033')
            {
                char seq[2];

                if (read(STDIN_FILENO, &seq[0], 1) > 0 &&
                    read(STDIN_FILENO, &seq[1], 1) > 0)
                {
                    if (seq[0] == '[')
                    {
                        if (seq[1] == 'A')
                        {
                            if (historyCount > 0 && historyIndex > 0)
                            {
                                historyIndex--;

                                while (pos > 0)
                                {
                                    printf("\b \b");
                                    pos--;
                                }

                                strcpy(input, history[historyIndex]);

                                pos = strlen(input);

                                printf("%s", input);

                                fflush(stdout);
                            }
                        }
                        else if (seq[1] == 'B')
                        {
                            
                            if (historyIndex < historyCount - 1)
                            {
                                historyIndex++;

                                while (pos > 0)
                                {
                                    printf("\b \b");
                                    pos--;
                                }

                                strcpy(input, history[historyIndex]);

                                pos = strlen(input);

                                printf("%s", input);

                                fflush(stdout);
                            }

                            
                            else if (historyIndex == historyCount - 1)
                            {
                                while (pos > 0)
                                {
                                    printf("\b \b");
                                    pos--;
                                }

                                input[0] = '\0';

                                historyIndex = historyCount;

                                fflush(stdout);
                            }
                        }
                    }
                }
            }

            else
            {
                if (pos < MAX_INPUT - 1)
                {
                    input[pos] = ch;

                    pos++;

                    input[pos] = '\0';

                    putchar(ch);

                    fflush(stdout);
                }
            }
        }

       
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        if (strlen(input) == 0)
        {
            continue;
        }

        addHistory(input);

        
        parseInput(input);

        
        pos = 0;

        input[0] = '\0';
    }

    return 0;
}