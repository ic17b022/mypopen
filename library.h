#ifndef MYPOPEN_LIBRARY_H
#define MYPOPEN_LIBRARY_H

FILE *mypopen(const char *command, const char *type);

int mypclose(FILE *stream);

void read_from_pipe (int file);

void write_to_pipe (int file);

#endif