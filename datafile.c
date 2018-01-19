/*
 * It contains the data file management functions for the
 * data manager. It includes functions to create data files
 * and to add, change, and delete records in data files
 */

#include <stdio.h>
#include <unistd.h>
#include "cdata.h"

#define flocate(r,l) ((long)(sizeof(FHEADER) + (((r)-1)*(l))))

static int handle[MXFILS];
FHEADER fh[MXFILS];

/****** create a file ******/
void file_create(char *name, int len) {
	int fp;
	FHEADER hd;

	/* delete a file name. If it's the file's only name, the file is deleted as well */
	unlink(name);
	fp = creat(name, CMODE);
	close(fp);
	fp = open(name, OPENMODE);

}
