#include "shell.h"

char history[MAX_HISTORY][MAX_INPUT];

int historyCount = 0;


void loadHistory()
{
    
    FILE *file = fopen(".history", "r");

    if (file == NULL)
    {
        perror("history");
        return;
    }

    char command[MAX_INPUT];
    
    while (fgets(command, MAX_INPUT, file) != NULL)
    {
        
        command[strcspn(command, "\n")] = '\0';

        if (strlen(command) == 0)
        {
            continue;
        }

        if (historyCount < MAX_HISTORY)
        {
            strcpy(history[historyCount], command);

            historyCount++;
        }
        else
        {
            for (int i = 1; i < MAX_HISTORY; i++)
            {
                strcpy(history[i - 1], history[i]);
            }

            strcpy(history[MAX_HISTORY - 1], command);
        }
    }

    fclose(file);
}

void saveHistory()
{
    
    FILE *file = fopen(".history", "w");

    if (file == NULL)
    {
        perror("history");
        return;
    }
    
    for (int i = 0; i < historyCount; i++)
    {
        fprintf(file, "%s\n", history[i]);
    }

    fclose(file);
}

void addHistory(char *command)
{
    if (strlen(command) == 0)
    {
        return;
    }

    if (historyCount < MAX_HISTORY)
    {
        strcpy(history[historyCount], command);

        historyCount++;
    }
    else
    {
        for (int i = 1; i < MAX_HISTORY; i++)
        {
            strcpy(history[i - 1], history[i]);
        }

        strcpy(history[MAX_HISTORY - 1], command);
    }

    saveHistory();
}

void runHistory(char **args, int argc)
{
    int count = 10;

    if (argc == 2)
    {
        count = atoi(args[1]);
    }

    if (count > historyCount)
    {
        count = historyCount;
    }

    int start = historyCount - count;

    for (int i = start; i < historyCount; i++)
    {
        printf("%s\n", history[i]);
    }
}