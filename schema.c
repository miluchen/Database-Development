/******************** schema.c *********************/
/* Read a file from the standard input.
 * Write one of three schema files to the standard output depending on the runtime switch:
 * If -1 is on the command line, the #define statements are built
 * If -2 is on the command line, the file of ascii strings for file and data element names is built
 * If -3 is on the command line, the data base schema array source file is built
 */

#include <stdio.h>
#include "cdata.h"

#define SCHEMA

#define MXCAT 3	/* maximum elements concatenated per index */
#define NAMLEN 31
#define REMARK ';'
#define MAXLINE 120

struct dict {				/* data element dictionary	*/
	char dename [NAMLEN+1];	/* name						*/
	char detype;			/* type						*/
	int delen;				/* length					*/
	char *demask;			/* display mask				*/
} dc [MXELE];

int dectr = 0;				/* data elements in dictionary	*/
int fctr  = 0;				/* files in data base			*/
char filename [MXFILS][NAMLEN+1];	/* file name strings	*/
int fileele [MXFILS][MXELE];		/* elements in files	*/
int ndxele [MXFILS][MXINDEX][MXCAT];/* indices				*/

char word[NAMLEN+1];
int lnctr = 0;	/* input stream line counter */
char line[160];

/****** error message *******/
char *ers[] = {
	"invalid name",					/* 1 */
	"invalid length",				/* 2 */
	"comma missing",				/* 3 */
	"invalid data type",			/* 4 */
	"quote missing",				/* 5 */
	"#schema missing",				/* 6 */
	"#<command> missing",			/* 7 */
	"unexpected end of file",		/* 8 */
	"duplicate file name",			/* 9 */
	"unknown data element",			/* 10 */
	"too many data elements",		/* 11 */
	"out of memory",				/* 12 */
	"unknow file name",				/* 13 */
	"too many indices in file",		/* 14 */
	"too many elements in index",	/* 15 */
	"duplicate data element",		/* 16 */
	"too many files",				/* 17 */
	"invalid command line switch"	/* 18 */
};

void get_line();
/****** main program ******/
int main(int argc, char *argv[]) {
	
}

static void get_line() {
	*line = '\0';
	while (*line == '\0' || *line == REMARK || *line == '\n') {
		if (fgets(line, MAXLINE, stdin) == 0) {
			error(8);
			depart(1);
		}
		lnctr++;
	}
}
