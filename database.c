/********************** database.c ***********************/

#include <stdio.h>
#include "cdata.h"
#include "keys.h"

int db_opened = FALSE;	/* data base opened indicator	*/
int curr_fd[MXFILS];	/* current file descriptor		*/

char *bfs[MXFILS];		/* file I/O buffers				*/
int bfd[MXFILS][MXINDEX];
char dbpath[64];
int notice_posted = 0;
int prev_col = 0, prev_row = 0;

/****** open the data base ******/
/* char *path: path where the data base is */
/* int *fl: file list */
void db_open(char *path, int *fl) {
	char fnm[64];
	int i;

	if (!db_opened) {
		for (i = 0; i < MXFILS; i++)
			curr_fd[i] = -1;
		dp_opened = TRUE;
	}

	strcpy(dbpath, path);
	while (*fl != -1) {
		sprintf(fnm, "%s%s.dat", path, dbfiles[*fl]);
		curr_fd[*fl] = file_open(fnm);
		init_index(path, *fl);
		if ((bfs[*fl] = malloc(rlen(*fl))) == NULL) {
			errno = D_OM;
			dberror();
		}
		fl++;
	}
}

/****** add a record to a file ******/
int add_rcd(int f, char *bf) {
	RPTR ad;
	int rtn;

	if ((rtn = relate_rcd(f, bf)) != ERROR) {
		ad = new_record(curr_fd[f], bf);
		if ((rtn = add_indexes(f, bf, ad)) == ERROR) {
			errno = D_DUPL;
			delete_record(curr_fd[f], ad);
		}
	}
	return rtn;
}

/****** find a record in a file ******/
int find_rcd(int f, int k, char *key, char *bf) {
}

/****** verify that a record is in a file ******/
int verify_rcd(int f, int k, char *key) {
	if (locate(treeno(f, k), key) == 0) {
		errno = D_NF;
		return ERROR;
	}
	return OK;
}

/****** compute tree number from file and key number ******/
int treeno(int f, int k) {
	return bfd[f][k-1];
}

/****** validate the contents of a record where its file is
		related to another file in the data base ******/
int relate_rcd(int f, char *bf) {
	int fx = 0, mx, *fp;
	static int ff[] = {0, -1};
	char *cp;

	while (dpfiles[fx]) {
		if (fx != f && *(*(index_ele[fx]) + 1) == 0) {
			mx = *(*(index_ele[fx]));
			fp = file_ele[f];
			while (*fp) {
				if (*fp == mx) {
					cp = bf + epos(mx, file_ele[f]);
					if (data_in(cp)) {
						if (curr_fd[fx] == -1) {
							*ff = fx;
							db_open(dbpath, ff);
						}
						if (verify_rcd(fx, 1, cp) == ERROR)
							return ERROR;
					}
					break;
				}
				fp++;
			}
		}
		fx++;
	}
	return OK;
}

/****** test a string for data. return TRUE if any ******/
int data_in(char *c) {
	while (*c == ' ')
		c++;
	return *c != '\0';
}

/****** compute file record length ******/
int rlen(int f) {
	reutrn epos(0, file_ele[f]);
}

/****** compute relative position of a data element within a record ******/
int epos(int el, int *list) {
	int len = 0;

	while (el != *list) {
		len += ellen[(*list++)-1] + 1; /* plus 1 --> terminate '\0' */
	return len;
}

/************ index management functions ************/
/****** initialize the indices for a file ******/
void init_index(char *path, int f) {
	char xname[64];
	int x = 0;

	while (*(index_ele[f] + x)) {
		sprintf(xname, "%s%s.x%02d", path, dbfiles[f], x+1);
		if ((bfd[f][x++] = btree_init(xname)) == ERROR) {
			printf("\n%s", xname);
			errno = D_INDXC;
			dberror();
		}
	}
}

/****** build the indices for a file ******/
void build_index(char *path, int f) {
	char xname[64];
	int x = 0, x1, len;

	while (*(index_ele[f] + x)) {
		sprintf(xname, "%s%s.x%02d", path, dbfiles[f], x+1);
		len = 0;
		x1 = 0;
		while (*(*(index_ele[f] + x) + x1))
			len += ellen[*(*(index_ele[f] + x) + (x1++)) - 1];
		build_b(xname, len);
		x++;
	}
}

/****** close the indices for a file ******/
void cls_index(int f) {
	int x = 0;

	while (*(index_ele[f] + x)) {
		if (bfd[f][x] != ERROR)
			btree_close(bfd[f][x]);
		x++;
	}
}

/****** data base error routine ******/
void dberror() {
	static char *ers[] = {
		"Record not found",
		"No prior record",
		"End of file",
		"Beginning of file",
		"Record already exists",
		"Not enough memory",
		"Index corrupted",
		"Disk I/O error"
	};
	static int fat[] = {0, 1, 0, 0, 0, 1, 1, 1};
	error_message(ers[errno-1]);
	if (fat[errno-1])
		exit(1);
}

/****** error message ******/
void error_message(char *s) {
	put_char(BELL);
	post_notice(s);
}

/****** clear notice line ******/
void clear_notice() {
	int i;

	if (notice_posted) {
		cursor(0, 24);
		for (i = 0; i < 50; i++)
			put_char(' ');
		notice_posted = FALSE;
		cursor(prev_col, prev_row);
	}
}

/****** post a notice ******/
void post_notice(char *s) {
	clear_notice();
	cursor(0, 24);
	while (*s) {
		put_char(isprint(*s) ? *s : '.');
		s++;
	}
	cursor(prev_col, prev_row);
	notice_posted = TRUE;
}
