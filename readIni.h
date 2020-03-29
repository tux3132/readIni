#ifndef READINI_H
#define READINI_H

#include <stdio.h>
#include <string.h>

extern char buffIni[40];

int ini_gets(const char *Section, const char *Key, const char *DefValue, char *Buffer, int BufferSize, const char *Filename);

#endif
