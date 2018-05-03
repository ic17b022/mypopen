#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include <assert.h>

#include "mypopen.h"

static int childID = MYPOPEN_NOCHILD;

FILE* mypopen(const char* command, const char* type) {
    int mypipe[2];

    if (childID != MYPOPEN_NOCHILD) {
        errno = EAGAIN;
        return NULL;
    }

    if (type == NULL || (*type != 'w' && *type != 'r')) {
        errno = EINVAL;
        return NULL;
    }

    if (pipe(mypipe) != 0)
        return NULL;    //errno remains set

    childID = fork();

    if (childID == -1) {
        return NULL;    //errno remains set
    } else if (childID == (pid_t) 0) {
        // child process
        if (*type == 'w') {
            //wenn write
            //close write end
            close(mypipe[MYPOPEN_WRITE]);
            //set fd to std.in
            dup2(mypipe[MYPOPEN_READ], STDIN_FILENO);
            execl("/bin/sh", "sh", "-c", command, (char*) NULL);
        } else if (*type == 'r') {
            //wenn read
            //close read end
            close(mypipe[MYPOPEN_READ]);
            //set fd to std.out
            dup2(mypipe[MYPOPEN_WRITE], STDOUT_FILENO);
            execl("/bin/sh", "sh", "-c", command, (char*) NULL);
        }
    } else {
        // parent process
        if (*type == 'w') {
            close(mypipe[MYPOPEN_READ]);
            return fdopen(mypipe[MYPOPEN_WRITE], type);
        } else if (*type == 'r') {
            close(mypipe[MYPOPEN_WRITE]);
            return fdopen(mypipe[MYPOPEN_READ], type);
        }
    }

    //if we got here things went very wrong
    assert(0);
}

int mypclose(void) {
    int waitReturn = waitpid(childID, NULL, 0);

    if (waitReturn == childID)
        childID = MYPOPEN_NOCHILD;

    return waitReturn;
}