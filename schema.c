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

/* utility functions */
static void depart(int);
static void error(int);
static void get_line();
static void skip_white(char **);
static void lcase(char *, char *);
static void *get_word(char *);
static void name_val();
static void numb_val();
static void expect_comma(char **);

/* build the dictionary, file and index spec, and write the files */
static void de_dict();
static void files();
static void keys();
static void schout();
static void defout();
static void strout();

/****** main program ******/
int main(int argc, char *argv[]) {
	get_line();
	if (strncmp(line, "#schema ", 8))
		error(6);
	else {
		get_word(line + 8);
		name_val();
		printf("\n/* ---------- %s ---------- */\n", word);
	}

	get_line();
	while (strncmp(line, "#end schema", 11)) {
		if (strncmp(line, "#dictionary", 11) == 0)
			de_dict();
		else if (strncmp(line, "#file ", 6) == 0)
			files();
		else if (strncmp(line, "#key ", 5) == 0)
			keys();
		else
			error(7);
		get_line();
	}

	if (argc > 1) {
		if (strcmp(argv[1], "-1") == 0)
			defout();
		else if (strnmp(argv[1], "-2") == 0)
			strout();
		else if (strcmp(argv[1], "-3") == 0)
			schout();
		else
			error(18);
	}
	else
		error(18);
	depart(0);
}

/****** build the data element dictionary ******/
void de_dict() {
	char *cp, *cp1;
	int el;
	while (TRUE) {

	}
}

/****** utility functions ******/
/* terminate schema program */
void depart(int n) {
	int el;
	for (el = 0; el < dectr; el++)
		free(dc[el].demask);
	exit(n);
}

/* errors */
void error(int n) {
	static int erct = 0;
	static int erlin = 0;

	if (erlin != lnctr) {
		erlin = lnctr;
		fprintf(stderr, "\nLine %d: %s", lnctr, line);
	}
	fprintf(stderr, "\nError %d: %s", n, ers[n-1]);
	if (erct++ == 5) {
		erct = 0;
		fprintf(stderr, "\nContinue? (y/n) ...");
		if (tolower(getc(stderr)) != 'y')
			depart(1);
	}
}

/* get a line of data from the schema input stream */
void get_line() {
	*line = '\0';
	while (*line == '\0' || *line == REMARK || *line == '\n') {
		if (fgets(line, MAXLINE, stdin) == 0) {
			error(8);
			depart(1);
		}
		lnctr++;
	}
}

/* skip white spaces */
void skip_white(char **s) {
	while (isspace(**s))
		(*s)++;
}

/* convert a name to lower case */
void lcase(char *s1, char *s2) {
	while (*s2) {
		*s1 = tolower(*s2);
		s1++;
		s2++;
	}
	*s1 = '\0';
}

/* get a word from a line of input */
char *get_word(char *cp) {
	int wl = 0, fst = 0;

	skip_white(&cp);
	while (*cp && *cp != '\n' && *cp != ',' && !isspace(*cp)) {
		if (wl == NAMLEN && fst == 0) {
			error(1);
			fst++;
		}
		else
			word[wl++] = *cp++;
	}
	word[wl] = '\0';
	return cp;
}

/* validate a name */
void name_val() {
	char *s = word;
	if (isalpha(*s)) {
		while (isalpha(*s) || isdigit(*s) || *s == '_') {
			*s = toupper(*s);
			s++;
		}
		if (*s == '\0')
			return;
	}
	error(1);
}

/*validate a number */
void numb_val() {
	char *s = word;
	do {
		if (!isdigit(*s++)) {
			error(2);
			break;
		}
	} while (*s);
}

/* expect a comman next */
void expect_comma(char **cp) {
	skip_white(cp);
	if (*(*cp)++ != ',')
		error(3);
}
