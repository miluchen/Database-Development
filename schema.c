/******************** schema.c *********************/
/* It is a simple compiler that compiles DDL statements into C statements
 * Read a file from the standard input.
 * Write one of three schema files to the standard output depending on the runtime switch:
 * If -1 is on the command line, the #define statements are built
 * If -2 is on the command line, the file of ascii strings for file and data element names is built
 * If -3 is on the command line, the data base schema array source file is built
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
static char *get_word(char *);
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
		else if (strcmp(argv[1], "-2") == 0)
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
		get_line();
		if(strncmp(line, "#end dictionary", 15) == 0)
			break;
		if (dectr == MXELE) {
			error(11);
			continue;
		}
		cp = get_word(line);
		name_val();
		for (el = 0; el < dectr; el++) {
			if (strcmp(word, dc[el].dename) == 0) {
				error(16);
				continue;
			}
		}
		strcpy(dc[dectr].dename, word);
		expect_comma(&cp);
		skip_white(&cp);
		switch (*cp) {
			case 'A':
			case 'Z':
			case 'C':
			case 'N':
			case 'D':
				break;
			default:
				error(4);
				continue;
		}
		dc[dectr].detype = *cp++;
		expect_comma(&cp);
		cp = get_word(cp);
		numb_val();
		dc[dectr].delen = atoi(word);
		expect_comma(&cp);
		skip_white(&cp);
		if (*cp != '"') {
			error(5);
			continue;
		}
		cp1 = cp + 1;
		while (*cp1 != '"' && *cp1 && *cp1 != '\n')
			cp1++;
		if (*cp1++ != '"') {
			error(5);
			continue;
		}
		*cp1 = '\0';
		if ((dc[dectr].demask = malloc((cp1-cp) + 1)) == 0) {
			error(12);
			depart(1);
		}
		strcpy(dc[dectr].demask, cp);
		dectr++;
	}
}

/****** build the file definitions ******/
void files() {
	int i, el = 0;
	if (fctr == MXFILS)
		error(17);
	/* get the file name and validate it */
	get_word(line + 6);
	name_val();
	for (i=0; i<fctr; i++) {
		if (strcmp(word, filename[i]) == 0)
			error(9);
	}
	strcpy(filename[fctr], word);
	/* process the file's data elements */
	while (TRUE) {
		get_line();
		if (strncmp(line, "#end file", 9) == 0)
			break;
		if (el == MXELE) {
			error(11);
			continue;
		}
		/* get a data element and check whether it's in the dictionary */
		get_word(line);
		for (i=0; i<dectr; i++) {
			if (strcmp(word, dc[i].dename) == 0)
				break;
		}
		if (i == dectr)
			error(10);
		else if (fctr < MXFILS)
			fileele[fctr][el++] = i+1; /* post to file */
	}
	if (fctr < MXFILS)
		fctr++;
}

/* build the index descriptions */
void keys() {
	char *cp;
	int f, el, x, cat = 0;
	/* get the file name and check whether it's in the schema */
	cp = get_word(line + 5);
	for (f = 0; f < fctr; f++) {
		if (strcmp(word, filename[f]) == 0)
			break;
	}
	if (f == fctr) {
		error(13);
		return;
	}
	for (x=0; x<MXINDEX; x++) {
		if (*ndxele[f][x] == 0)
			break;
	}
	if (x == MXINDEX) {
		error(14);
		return;
	}
	while (cat < MXCAT) {
		/* get the index name and check whether it's in the dict */
		cp = get_word(cp);
		for (el = 0; el < dectr; el++) {
			if (strcmp(word, dc[el].dename) == 0)
				break;
		}
		if (el == dectr) {
			error(10);
			break;
		}
		ndxele[f][x][cat++] = el + 1; /* post element */
		skip_white(&cp);
		if (*cp++ != ',')	/* check if concatenated index */
			break;
		if (cat == MXCAT) {
			error(15);
			break;
		}
	}
}

/****** write the schema source language ******/
void schout() {
	int f, el, x, x1, cat, fel;
	char name[NAMLEN+1];

	/* data element lengths */
	printf("\n\nint ellen[] = {");
	for (el = 0; el < dectr; el++) {
		if ((el % 25) == 0)
			printf("\n\t");
		printf((el < dectr-1 ? "%d," : "%d"), dc[el].delen);
	}
	printf("\n};\n");
	/* write the file contents arrays */
	for (f = 0; f < fctr; f++) {
		lcase(name, filename[f]);
		printf("\n\nint f_%s[] = {", name);
		el = 0;
		while ((fel = fileele[f][el++]) != 0)
			printf("\n\t%s,", dc[fel-1].dename);
		printf("\n\t0\n};");
	}
	/* write the file list pointer array */
	printf("\n\nint *file_ele[] = {");
	for (f = 0; f < fctr; f++) {
		lcase(name, filename[f]);
		printf("\n\tf_%s,", name);
	}
	printf("\n\t0\n};\n");
	/* write the index arrays */
	for (f = 0; f < fctr; f++) {
		lcase(name, filename[f]);
		for (x = 0; x < MXINDEX; x++) {
			if (*ndxele[f][x] == 0)
				break;
			printf("\nint x%d_%s[] = {", x+1, name);
			for (cat = 0; cat < MXCAT; cat++)
				if (ndxele[f][x][cat])
					printf("\n\t%s,", dc[ndxele[f][x][cat]-1].dename);
			printf("\n\t0\n};\n");
		}
		printf("\nint *x_%s[] = {", name);
		for (x1 = 0; x1 < x; x1++)
			printf("\n\tx%d_%s,", x1+1, name);
		printf("\n\t0\n};\n");
	}
	printf("\nint **index_ele[] = {");
	for (f = 0; f < fctr; f++) {
		lcase(name, filename[f]);
		printf("\n\tx_%s,", name);
	}
	printf("\n\t0\n};\n");
}

/****** write the schema #defines and struct definitions ******/
void defout() {
	int f, el, fel;
	char name[NAMLEN+1];

	/* data element defines */
	for (el = 0; el < dectr; el++)
		printf("\n#define %s %d", dc[el].dename, el + 1);
	putchar('\n');
	/* write the file #define statements */
	for (f = 0; f < fctr; f++)
		printf("\n#define %s %d", filename[f], f);
	putchar('\n');
	/* write the record structures */
	for (f = 0; f < fctr; f++) {
		lcase(name, filename[f]);
		printf("\nstruct %s {", name);
		el = 0;
		while ((fel = fileele[f][el++]) != 0) {
			lcase(name, dc[fel-1].dename);
			printf("\n\tchar %s [%d];", name, dc[fel-1].delen+1);
		}
		printf("\n};\n");
	}
}

/* write the file and data element ascii strings */
void strout() {
	int el, f;

	/* data element ascii names */
	printf("\nchar *denames[] = {");
	for (el = 0; el < dectr; el++)
		printf("\n\t\"%s\",", dc[el].dename);
	printf("\n\t0\n};\n");
	/* data element types */
	printf("\nchar eltype[] = \"");
	for (el = 0; el < dectr; el++)
		putchar(dc[el].detype);
	printf("\";\n");
	/* data element display masks */
	printf("\nchar *elmask[] = {");
	for (el = 0; el < dectr; el++)
		printf((el < dectr-1 ? "\n\t%s," : "\n\t%s"), dc[el].demask);
	printf("\n};\n");
	/* write the ascii file name strings */
	printf("\nchar *dbfiles[] = {");
	for (f = 0; f < fctr; f++)
		printf("\n\t\"%s\",", filename[f]);
	printf("\n\t0\n};\n");
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
