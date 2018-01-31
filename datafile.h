#ifndef _DATAFILE_H_
#define _DATAFILE_H_

#include "cdata.h"

void file_create(char *name, int len);
int file_open(char *name);
void file_close(int fp);
RPTR new_record(int fp, char *bf);
int get_record(int fp, RPTR rcdno, char *bf);
int put_record(int fp, RPTR rcdno, char *bf);
int delete_record(int fp, RPTR rcdno);

#endif
