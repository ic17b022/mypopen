#ifndef MYPOPEN_LIBRARY_H
#define MYPOPEN_LIBRARY_H

#include <stdio.h>

FILE *mypopen(const char *command, const char *type)
{
    //create pipe
    //fork process
    //check if pipe already exists

    /* parent process
     *
     *  read or write from/to pipe invers zum child
     *
     */

    /* child process
     *
     * dup2
     * set correct end of pipe depending read or write
     * execv/execl zum ausfuehren des kommandos mit sh
     * nach exec - exit() mit error weil dieser code nicht mehr ausgefuert werden sollte
     *
     */



    return popen(command, type);
}

int mypclose(FILE *stream)
{

    /* parent process
     *
     * Bei Aufruf von mypclose() soll der aufrufende Prozeß auf die Terminierung des Kindprozesses
     * warten (waitpid(2)). Zur Vereinfachung soll immer nur höchstens eine Pipe mit mypopen()
     * geöffnet werden können. Stellen Sie dies in ihrer Implementierung sicher.
     *
     * fclose (stream)
     */

    /* child process
     *
     */
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

#endif