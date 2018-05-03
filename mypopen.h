#ifndef MYPOPEN_H
#define MYPOPEN_H

#include <stdio.h>

#define MYPOPEN_READ 0
#define MYPOPEN_WRITE 1

#define MYPOPEN_NOCHILD -2

FILE* mypopen(const char* command, const char* type);

int mypclose(void);

#endif