#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include <assert.h>
#include <stdlib.h>

#include "mypopen.h"

static FILE *fp_stream = NULL;
static int childID = MYPOPEN_NOCHILD;
static int mypipe[2];

FILE* mypopen(const char* command, const char* type) {
    if (childID != MYPOPEN_NOCHILD) {
        errno = EAGAIN;
        return NULL;
    }

    if (type == NULL || (strcmp(type, "w") != 0 && strcmp(type, "r") != 0)) {
        errno = EINVAL;
        return NULL;
    }

    if (pipe(mypipe) != 0)
        return NULL;    //errno remains set

    childID = fork();

    if (childID == MYPOPEN_NOCHILD) {
        close(mypipe[MYPOPEN_READ]);        //#TODO: error handling?
        close(mypipe[MYPOPEN_WRITE]);

	return NULL;
    } else if (childID == (pid_t) 0) {
        // child process
        if (*type == 'w') {
            //wenn write
            //close write end
            close(mypipe[MYPOPEN_WRITE]);
            //set fd to std.in
            if(dup2(mypipe[MYPOPEN_READ], STDIN_FILENO) == -1){
	    close(mypipe[MYPOPEN_READ]);
	    _Exit(EXIT_FAILURE);
	    }

            execl("/bin/sh", "sh", "-c", command, (char*) NULL);
            _Exit(EXIT_FAILURE);
        } else if (*type == 'r') {
            //wenn read
            //close read end
            close(mypipe[MYPOPEN_READ]);
            //set fd to std.out
            if(dup2(mypipe[MYPOPEN_WRITE], STDOUT_FILENO) == -1){
	    close(mypipe[MYPOPEN_WRITE]);
	    _Exit(EXIT_FAILURE);
	    }

            execl("/bin/sh", "sh", "-c", command, (char*) NULL);
            _Exit(EXIT_FAILURE);
        }
    } else {
        // parent process
        if (*type == 'w') {
            close(mypipe[MYPOPEN_READ]);
	    if((fp_stream = fdopen(mypipe[MYPOPEN_WRITE], type)) == NULL){
	//error handling if fdopen fails
	    close(mypipe[MYPOPEN_WRITE]);
	    childID = MYPOPEN_NOCHILD;
	    return NULL;
	    }
            return fp_stream;
	} else if (*type == 'r') {
            close(mypipe[MYPOPEN_WRITE]);
            if((fp_stream = fdopen(mypipe[MYPOPEN_READ], type)) == NULL){
	//error handling if fdopen fails
	    close(mypipe[MYPOPEN_READ]);
            childID = MYPOPEN_NOCHILD;
	    return NULL;
	    }
	    return fp_stream;
        }
    }

    //if we got here things went very wrong
    assert(0);
}


//#TODO: ordentlich Fehlerbehandlung. pclose returned -1 on error und setzt errno. Was machen wir denn wenn fflush schief geht? Sofort -1 retournieren? Dann bleibt die pipe offen und der child ist uns auch wurst. Dafür könnte der aufrufer es nochmal probieren.
// oder machen wir den Rest trotzdem und dann ist die pipe eben zu und was auch immer geflushed hätte werden sollen hat Pech gehabt?
int mypclose(FILE* stream) {

int status;
int waitReturn;

    if (childID == MYPOPEN_NOCHILD) {
        errno = ECHILD;
        return -1;
}
   
    if(fp_stream != stream) {
	errno = EINVAL;
	return -1;
	}
    fflush(stream);

    if (fclose(stream) == EOF) {/*errno is set by fclose*/
        fp_stream = NULL;
        childID = MYPOPEN_NOCHILD;
        return -1;
	}

    //return value and errno intentionally ignored. Either we are in write mode, then this will be fine. Or we are not and I don't care that closing this failed.
    close(mypipe[MYPOPEN_WRITE]);
    //handling if waitpid gets interupted (Test 30)	
    do{	
    waitReturn = waitpid(childID, &status, 0);
}while(waitReturn == -1 && errno == EINTR);


    if (waitReturn == childID)
        childID = MYPOPEN_NOCHILD;
    // check if waitpid worked correctly
    if (waitReturn == -1){
        fp_stream = NULL;
        childID = MYPOPEN_NOCHILD;
        errno = ECHILD;
        return -1;
    }
    //check if child process exited correctly if yes return this exit code
    if (WIFEXITED(status)) {
        childID = MYPOPEN_NOCHILD;
        fp_stream = NULL;
	return WEXITSTATUS(status);
    }

    //if process did not exit correctly return ECHILD
    childID = MYPOPEN_NOCHILD;
    fp_stream = NULL;
    errno = ECHILD;

    return -1;   
}
