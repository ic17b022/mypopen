#include <stdio.h>
#include <unistd.h>
#include <string.h>



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
        * execv/execl zum ausfuehren des kommandos mit sh
        *
        *
        */

        if (*type == 'w') {
            //wenn write
            //close write end
            close(mypipe[1]);
            //set fd to std.in
            dup2(mypipe[0],stdin);
        } else if (*type == 'r') {
            //wenn read
            //close read end
            close(mypipe[0]);
            //set fd to std.out
            dup2(mypipe[1],stdout);
        } else {
            //wenn falsch
            //Im Kindprozeß ist das richtige Ende der Pipe ("r" oder "w") mit stdin bzw. stdout zu assoziieren (dup2(2))
        }


    }
    else {
            // parent process
        if (*type == 'w') {
            close(mypipe[0]);
            return fdopen(mypipe[1],type);
        } else if (*type == 'r') {
            close(mypipe[1]);
            return fdopen(mypipe[0],type);
        } else {
            //wenn falsch
            //Im Kindprozeß ist das richtige Ende der Pipe ("r" oder "w") mit stdin bzw. stdout zu assoziieren (dup2(2))
        }

        */



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