/*
 * It contains the data file management functions for the
 * data manager. It includes functions to create data files
 * and to add, change, and delete records in data files
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
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
	hd.first_record = 0;
	hd.next_record = 1;
	hd.record_length = len;
	write(fp, (char *)&hd, sizeof(hd));
	close(fp);
}

/****** open a file ******/
int file_open(char *name) {
	int fp;

	for (fp = 0; fp < MXFILS; fp++)
		if (handle[fp] == 0)
			break;
	if (fp == MXFILS)
		return ERROR;
	if ((handle[fp] = open(name, OPENMODE)) == ERROR)
		return ERROR;
	lseek(handle[fp], 0L, 0);
	read(handle[fp], (char *)&fh[fp], sizeof(FHEADER));
	return fp;
}

/****** close a file ******/
void file_close(int fp) {
	lseek(handle[fp], 0L, 0);
	write(handle[fp], (char *)&fh[fp], sizeof(FHEADER));
	close(handle[fp]);
	handle[fp] = 0;
}

/****** retrieve a record ******/
int get_record(int fp, RPTR rcdno, char *bf) {
	if (rcdno >= fh[fp].next_record)
		return ERROR;
	lseek(handle[fp], flocate(rcdno, fh[fp].record_length), 0);
	read(handle[fp], bf, fh[fp].record_length);
	return OK;
}

/****** rewrite a record ******/
int put_record(int fp, RPTR rcdno, char *bf) {
	if (rcdno >= fh[fp].next_record)
		return ERROR;
	lseek(handle[fp], flocate(rcdno, fh[fp].record_length), 0);
	write(handle[fp], bf, fh[fp].record_length);
	return OK;
}

/****** create a new record ******/
RPTR new_record(int fp, char *bf) {
	RPTR rcdno; /* logical record number, relative to 1 */
	FHEADER *c;

	if (fh[fp].first_record) {
		rcdno = fh[fp].first_record;
		if ((c = (FHEADER *)malloc(fh[fp].record_length)) == NULL) {
			errno = D_OM;
			dberror();
		}
		get_record(fp, rcdno, (char *)c);
		fh[fp].first_record = c->next_record;
		free(c);
	}
	else
		rcdno = fh[fp].next_record++;
	put_record(fp, rcdno, bf);
	return rcdno;
}

/****** delete a record ******/
int delete_record(int fp, RPTR rcdno) {
	FHEADER *bf;

	if (rcdno >= fh[fp].next_record)
		return ERROR;
	if ((bf = (FHEADER *)malloc(fh[fp].record_length)) == NULL) {
		errno = D_OM;
		dberror();
	}
	memset(bf, 0, fh[fp].record_length);
	bf->next_record = fh[fp].first_record;
	bf->first_record = -1;
	fh[fp].first_record = rcdno;
	put_record(fp, rcdno, (char *)bf);
	free(bf);
	return OK;
}
