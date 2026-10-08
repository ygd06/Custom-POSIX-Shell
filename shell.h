#ifndef SHELL_H
#define SHELL_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <climits>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <grp.h>
#include <fcntl.h>
#include <ctime>
#include <signal.h>
#include <termios.h>

#define MAX_INPUT 4096

extern char homedir[PATH_MAX];
extern char prevdir[PATH_MAX];
extern volatile sig_atomic_t foregroundPid;

void initShell();

void printPrompt();

void parseInput(char *input);

void parseCmd(char *cmd);

void runCmd(char **args, int argc);

void runPwd();

void runEcho(char **args, int argc);

void runCd(char **args, int argc);

void runLs(char **args, int argc);

void listDir(char *path, int showall, int longlist);

void listLong(char *path, char *name);

void runSysCmd(char **args, int argc);

void runPinfo(char **args, int argc);

void runSearch(char **args, int argc);

int searchFile(char *path, char *name);

void runRedirect(char **args, int argc, char *infile, char *outfile, int append);

int setupInput(char *file);

int setupOutput(char *file, int append);

void runPipe(char *cmd);

void handleSigint(int sig);

void handleSigtstp(int sig);

void handleSigchld(int sig);

void autocomplete(char *input, int &pos);

#define MAX_HISTORY 20

extern char history[MAX_HISTORY][MAX_INPUT];
extern int historyCount;

void loadHistory();

void saveHistory();

void addHistory(char *command);

void runHistory(char **args, int argc);

#endif