//*
// @file mypopen.c
//
// Betriebssysteme - Beispiel 2
// mypopen - a simplified version of popen(3)
//
// mypopen recreates most of the functionality of popen(3)
//
// @author Manuel Seifner	 <ic17b022@technikum-wien.at>
// @author Oliver Safar		 <ic17b077@technikum-wien.at>
// @date 2018/05/4
//
// @version 1.0
//
//
// -------------------------------------------------------------- includes --
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include <assert.h>
#include <stdlib.h>

#include "mypopen.h"

// --------------------------------------------------------------- defines --

// -------------------------------------------------------------- typedefs --

// --------------------------------------------------------------- globals --

static FILE* fp_stream = NULL;
static int childID = MYPOPEN_NOCHILD;
static int mypipe[2];

// ------------------------------------------------------------- functions --

/**
 * \brief A simple version of the popen(3) function
 *
 * mypopen creates a pipe, forks, and invokes the shell.  Since a pipe is
 * by definition unidirectional, the type argument may specify only reading or writing, not both; the  result‐
 * ing stream is correspondingly read-only or write-only.
 *
 * The  command  argument is a pointer to a null-terminated string containing a shell command line.  This com‐
 * mand is passed to /bin/sh using the -c flag; interpretation, if any, is performed by the shell.
 *
 * The type argument is a pointer to a null-terminated string which must contain either  the  letter  'r'  for
 * reading  or the letter 'w' for writing. Otherwise NULL is returned and errno set to EINVAL.
 *
 * This popen implementation can only handle one child process at a time. If mypopen is called a second time before
 * mypclose is called NULL is returned and errno set to EAGAIN.
 *
 *
 * \param   command    the command to be passed to the shell
 * \param   type       either "w" or "r"
 *
 *
 * \return  FILE*
 * \retval  FILE* to one end of the pipe
 * \retval  NULL if an error occurs
 */
//#TODO: Error Handling für die ganzen close(). Doxygen nicht vergessen
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
        close(mypipe[MYPOPEN_READ]);        //no error handling here. We are already in error state and returning NULL.
        close(mypipe[MYPOPEN_WRITE]);
        return NULL;
    } else if (childID == 0) {
        // child process
        if (*type == 'w') {
            close(mypipe[MYPOPEN_WRITE]);

            if (mypipe[MYPOPEN_READ] != STDIN_FILENO) {
                if (dup2(mypipe[MYPOPEN_READ], STDIN_FILENO) == -1)
                    _Exit(EXIT_FAILURE);                //#TODO: hier wird die pipe nicht geschlossen -> umbauen auf single exit

                close(mypipe[MYPOPEN_READ]);
            }

            execl("/bin/sh", "sh", "-c", command, (char*) NULL);
            _Exit(EXIT_FAILURE);
        } else if (*type == 'r') {
            //wenn read
            //close read end
            close(mypipe[MYPOPEN_READ]);
            //set fd to std.out
            if (mypipe[MYPOPEN_WRITE] != STDOUT_FILENO) {
                if (dup2(mypipe[MYPOPEN_WRITE], STDOUT_FILENO) == -1)
                    _Exit(EXIT_FAILURE);

                close(mypipe[MYPOPEN_WRITE]);
            }

            execl("/bin/sh", "sh", "-c", command, (char*) NULL);
            _Exit(EXIT_FAILURE);
        }
    } else {
        // parent process
        if (*type == 'w') {
            close(mypipe[MYPOPEN_READ]);
            if ((fp_stream = fdopen(mypipe[MYPOPEN_WRITE], type)) == NULL) {
                //error handling if fdopen fails
                close(mypipe[MYPOPEN_WRITE]);
                childID = MYPOPEN_NOCHILD;
                return NULL;
            }
            return fp_stream;
        } else if (*type == 'r') {
            close(mypipe[MYPOPEN_WRITE]);
            if ((fp_stream = fdopen(mypipe[MYPOPEN_READ], type)) == NULL) {
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

/**
 * \brief A simple version of the pclose(3) function
 *
 *  The pclose() function waits for the associated process to terminate and returns the exit status of the command as
 *  returned by waitpid(2).
 *
 *  returns -1 on error.
 *  if there is no childprocess to wait on or it returned an error errno is set to ECHILD
 *  if the wrong filestream is passed errno is set to EINVAL
 *
 *
 *
 * \param   stream    the filestream to be closed
 *
 *
 * \return  int
 * \retval  status The exit code of the childprocess
 */
//#TODO: ordentlich Fehlerbehandlung. pclose returned -1 on error und setzt errno. Was machen wir denn wenn fflush schief geht? Sofort -1 retournieren? Dann bleibt die pipe offen und der child ist uns auch wurst. Dafür könnte der aufrufer es nochmal probieren.
// oder machen wir den Rest trotzdem und dann ist die pipe eben zu und was auch immer geflushed hätte werden sollen hat Pech gehabt?
int mypclose(FILE* stream) {

    int status;
    int waitReturn;

    if (childID == MYPOPEN_NOCHILD) {
        errno = ECHILD;
        return -1;
    }

    if (fp_stream != stream) {
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
    do {
        waitReturn = waitpid(childID, &status, 0);
    } while (waitReturn == -1 && errno == EINTR);


    if (waitReturn == childID)
        childID = MYPOPEN_NOCHILD;
    // check if waitpid worked correctly
    if (waitReturn == -1) {
        fp_stream = NULL;
        childID = MYPOPEN_NOCHILD;
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
// =================================================================== eof ==

// Local Variables:
// mode: c
// c-mode: k&r
// c-basic-offset: 8
// indent-tabs-mode: t
// End: