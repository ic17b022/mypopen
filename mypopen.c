#include <stdio.h>
#include <unistd.h>


FILE *mypopen(const char *command, const char *type)
{
    pid_t pid;
    int mypipe[2];


    pipe(mypipe);

    pid = fork();

    if (pid == (pid_t) 0) {

        if (strcmp(type, "w")=0) {
            //wenn write
            //close write end
            //set fd to std.in
            dup2();
            //Im Kindprozeß ist das richtige Ende der Pipe ("r" oder "w") mit stdin bzw. stdout zu assoziieren (dup2(2))
        } else if (strcmp(type, "r")=0) {
            //wenn read
            //close read end
            //set fd to std.out
            //Im Kindprozeß ist das richtige Ende der Pipe ("r" oder "w") mit stdin bzw. stdout zu assoziieren (dup2(2))
        } else {
            //wenn falsch
            //Im Kindprozeß ist das richtige Ende der Pipe ("r" oder "w") mit stdin bzw. stdout zu assoziieren (dup2(2))
        }

        /* child process
            *
            * dup2
            * read or write from/to pipe
            * execv/execl zum ausfuehren des kommandos mit sh
            *
            *
            */
    }
    else {
            /* parent process
            *
            */
        if (strcmp(type, "w")=0) {
            //wenn write
            //close read end
            //set fd to std.out
            dup2();
            //Im Kindprozeß ist das richtige Ende der Pipe ("r" oder "w") mit stdin bzw. stdout zu assoziieren (dup2(2))
        } else if (strcmp(type, "r")=0) {
            //wenn read
            //close write end
            //set fd to std.in
            //Im Kindprozeß ist das richtige Ende der Pipe ("r" oder "w") mit stdin bzw. stdout zu assoziieren (dup2(2))
        } else {
            //wenn falsch
            //Im Kindprozeß ist das richtige Ende der Pipe ("r" oder "w") mit stdin bzw. stdout zu assoziieren (dup2(2))
        }




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