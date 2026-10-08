# POSIX Shell Implementation

## Overview

This project implements a basic POSIX-style shell in C++.

The shell provides support for:

- Shell prompt with username, hostname, and current working directory
- Built-in commands
- External/system commands
- Foreground and background processes
- Input/output redirection
- Pipelines
- Signal handling
- Recursive file search
- Command and file autocomplete using TAB
- Persistent command history
- Up/down arrow history navigation

## Features Implemented

### 1. Shell Prompt

The shell displays a prompt in the following format:

username@system_name:current_directory

The starting directory of the shell is treated as the home directory and represented by `~`.

For example:

user@hostname:~$
user@hostname:~/folder$


The prompt is generated dynamically using the current username, hostname, and working directory.

### 2. Built-in Commands

The following built-in commands are implemented without using `execvp()`:

- `pwd`
- `echo`
- `cd`

#### `pwd`

Prints the current working directory.

#### `echo`

Prints the supplied arguments.

#### `cd`

Changes the current working directory.

Supported cases include:

- `cd`
- `cd ~`
- `cd .`
- `cd ..`
- `cd -`
- Normal directory paths

The previous directory is maintained to support `cd -`.

### 3. `ls`

The shell provides its own implementation of `ls`.

Supported options include:

- `ls`
- `ls -a`
- `ls -l`
- `ls -al`
- `ls -la`

Multiple files/directories and combinations of flags are supported.

### 4. External/System Commands

Commands other than the implemented built-ins are executed as external programs.

The shell creates a child process using `fork()` and executes the requested command using `execvp()`.

Both foreground and background execution are supported.

Example:

sleep 10

runs in the foreground, while:

sleep 10 &

runs in the background.

For a background process, the shell prints the process ID and immediately returns to the prompt.

### 5. Process Information

The `pinfo` command displays information about a process using the `/proc` filesystem.

Example:

pinfo
pinfo 1234

The implementation obtains information such as:

- Process ID
- Process state
- Virtual memory size
- Executable path

The foreground process is marked using `+` in its state when applicable.

### 6. Recursive Search

The `search` command recursively searches for a file or directory from the current directory.

Example:


search filename

The command prints:

True

when the requested item is found and:

False

otherwise.

### 7. I/O Redirection

The shell supports standard input and output redirection.

#### Input redirection


command < input.txt


#### Output redirection


command > output.txt


The `>` operator creates the output file if necessary and overwrites an existing file.

#### Append redirection

command >> output.txt

The `>>` operator appends output to an existing file.

Input and output redirection can also be used together.

The implementation uses file descriptors and `dup2()` to redirect standard input and output.

### 8. Pipes

Pipelines are supported using the `|` operator.

Example:

ls | grep cpp

Multiple commands can be connected:

cat file.txt | grep hello | wc -l

The shell creates pipes between processes so that the standard output of one command becomes the standard input of the next command.

Redirection can also be used inside pipeline commands.

### 9. Signal Handling

The shell handles the following signals:

#### Ctrl-C / SIGINT

Sends `SIGINT` to the currently running foreground process.

If there is no foreground process, Ctrl-C does not terminate the shell.

#### Ctrl-Z / SIGTSTP

Sends `SIGTSTP` to the currently running foreground process group.

Foreground processes are placed in their own process group, so Ctrl-Z can stop the entire foreground job, including all processes in a pipeline.

After the foreground job is stopped, the shell regains terminal control and displays the prompt again.

If there is no foreground process, Ctrl-Z has no effect on the shell.

#### SIGCHLD

The shell handles completed background child processes so that they do not remain as zombies.

#### Ctrl-D / EOF

Ctrl-D causes the shell to exit.

### 10. TAB Autocomplete

TAB autocomplete is supported for:

- Shell built-in commands
- Executable commands available through `PATH`
- Files in the current directory
- Directories in the current directory

When possible, the shell completes the common prefix of matching entries.

### 11. Command History

The shell maintains a persistent history of commands.

Features include:

- Stores up to 20 commands
- Displays at most 10 commands using `history`
- `history <num>` displays the requested number of commands
- Oldest entries are overwritten when the history limit is reached
- History is saved between shell sessions
- Up and Down arrow keys allow navigation through previous commands
- History entries are stored in the `.history` file

## Project Structure

main.cpp
shell.h
parser.cpp
process.cpp
builtins.cpp
pipe.cpp
autocomplete.cpp
history.cpp
features.cpp
Makefile
README.md


## Compilation and Execution

Compile the shell using:

make

Run it using:

make run

To remove generated build files:

make clean
