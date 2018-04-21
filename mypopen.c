#include <stdio.h>
#include <unistd.h>


FILE *mypopen(const char *command, const char *type)
{
    pid_t pid;
    int mypipe[2];


    pipe(mypipe);

    pid = fork();

    if (pid == (pid_t) 0) {
            /* child process
            *
            * dup2
            * read or write from/to pipe
            * wenn w
            * wenn r
            * execv/execl zum ausfuehren des kommandos mit sh
            *
            *
            */
    }
    else {
            /* parent process
            *
            */

            return fdopen(,type)
    }


    return popen(command, type);
}

int mypclose(FILE *stream)
{
    return pclose(stream);
}

void read_from_pipe (int file)
{
    FILE *stream;
    int c;
    stream = fdopen (file, "r");
    while ((c = fgetc (stream)) != EOF)
        // implement output of read data
        fclose (stream);
}

void write_to_pipe (int file)
{
    FILE *stream;
    stream = fdopen (file, "w");
    //implement reading of data
    fclose (stream);
}