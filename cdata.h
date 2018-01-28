/************************
 *	global definitions  *
 ************************/

#define ERROR -1
#define OK 0

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#include <errno.h>
#include <fcntl.h>
#include <ctype.h>

#define CMODE 0666
#define OPENMODE O_RDWR

#define NODE	512		/* length of a B-tree node			*/
#define RPTR	long	/* B-tree node and file address		*/
#define MXFILS	11		/* maximum files in a data base		*/
#define MXELE	100		/* maximum data elements in a file	*/
#define MXINDEX 5		/* maximum indeices per file		*/

/************* DBMS error codes for errno return ************/
#define D_NF	1		/* record not found					*/
#define D_PRIOR	2		/* no prior record for this request	*/
#define D_EOF	3		/* end of file						*/
#define D_BOF	4		/* beginning of file				*/
#define D_DUPL	5		/* primary key already exists		*/
#define D_OM	6		/* out of memory					*/
#define D_INDXC	7		/* index corrupted					*/
#define D_IOERR	8		/* I/O error						*/

#define MXKEYLEN 80		/* maximum key length for indices	*/

#ifndef SCHEMA
/************ schema as built for the application ***********/
extern char *dbfiles [];	/* file names					*/
extern char *denames [];	/* data element names			*/
extern char *elmask [];		/* data element masks			*/
extern char eltype [];		/* data element types			*/
extern int ellen [];		/* data element lengths			*/
extern int *file_ele [];	/* file data elements			*/
extern int **index_ele [];	/* index data elements			*/

/******************* helper functions *************************/
void mov_mem();
void cls_file();
void put_char();

/************* data base function definitions ***************/
void db_open(char *, int *);
void db_cls();
void dberror();
void init_rcd(int, char *);
void clrrcd(char *, int *);
void rcd_fill(char *, char *, int *, int *);
void build_index();

/************* screen dirver function definitions ***************/

#endif

/********************* file header **************************/
typedef struct fhdr {	/* header on each file */
	RPTR first_record;	/* first available deleted record */
	RPTR next_record;	/* next available record position */
	int record_length;	/* length of record */
} FHEADER;
