#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "mypopen.h"

FILE *mypopen(const char *command, const char *type)
{
    pid_t pid;
    int mypipe[2];

    if (*type != 'w' && *type != 'r'){
        errno = EINVAL;
        return NULL;
    }

    if(pipe(mypipe) != 0)
        return NULL;    //errno remains set

    pid = fork();

    if (pid == -1){
        return NULL;    //errno remains set
    } else if (pid == (pid_t) 0) {

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
        }
    }


    return popen(command, type);
}

int mypclose(FILE *stream)
{
    return pclose(stream);
}