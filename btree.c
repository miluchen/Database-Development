/*
 * This file contains functions that manage the data base B-tree index files
 */

#include <stdio.h>
#include "cdata.h"

#define MXTREES 20
#define ADR sizeof(RPTR)
#define KLEN bheader[trx].keylength
#define ENTLN (KLEN+ADR)

/****** the btree node structure ******/
typedef struct treenode {
	int nonleaf;	/* 0 if leaf, 1 if non-leaf */
	RPTR prntnode;	/* parent node				*/
	RPTR lfsib;		/* left sibling node		*/
	RPTR rtsib;		/* right sibling node		*/
	int keyct;		/* number of keys			*/
	RPTR key0;		/* node # of keys < 1st key this node	*/
	char keyspace[NODE - ((sizeof(int) * 2) + (ADR * 4))];
	char spil[MXKEYLEN];	/* for insertion excess */
} BTREE;

/****** the structure of the btree header node ******/
typedef struct treehdr {
	RPTR rootnode;	/* root node number			*/
	int keylength;
	int m;			/* max keys/node			*/
	RPTR rlsed_node;/* next released node		*/
	RPTR endnode;	/* next unassigned node		*/
	int locked;		/* if btree is locked		*/
	RPTR leftmost;	/* left-most node			*/
	RPTR rightmost;	/* right-most node			*/
} HEADER;

HEADER bheader[MXTREES];
BTREE trnode;

int handle[MXTREES];	/* handle of each index in use */
RPTR currnode[MXTREES];	/* node number of current key */
int currkno[MXTREES];	/* key number of current key */
int trx;				/* current tree */

/****** initiate b-tree processing ******/
int btree_init(char *ndx_name) {
	for (trx = 0; trx < MXTREES; trx++)
		if (handle[trx] == 0)
			break;
	if (trx == MXTREES)
		return ERROR;
	if ((handle[trx] = open(ndx_name, OPENMODE)) == ERROR)
		return ERROR;
	lseek(handle[trx], 0L, 0);
	read(handle[trx], (char *)&bheader[trx], sizeof(HEADER));
	if (bheader[trx].locked) {
		close(handle[trx]);
		handle[trx] = 0;
		return ERROR;
	}
	bheader[trx].locked = TRUE;
	lseek(handle[trx], 0L, 0);
	write(handle[trx], (char *)&bheader[trx], sizeof(HEADER));
	currnode[trx] = 0;
	currkno[trx] = 0;
	return trx;
}

/****** terminate b-tree processing ******/
int btree_close(int tree) {
	if (tree >= MXTREES || handle[tree] == 0)
		return ERROR;
	bheader[tree].locked = FALSE;
	lseek(handle[tree], 0L, 0);
	write(handle[tree], (char *)&bheader[tree], sizeof(HEADER));
	close(handle[tree]);
	handle[tree] = 0;
	return OK;
}

/****** build a new b-tree ******/
void build_b(char *name, int len) {
	HEADER *bhdp;
	int fd;

	if ((bhdp = (HEADER *)malloc(NODE)) == NULL) {
		errno = D_OM;
		dberror();
	}
	memset(bhdp, 0, NODE);
	bhdp->keylength = len;
	bhdp->m = ((NODE - ((sizeof(int) * 2) + (ADR * 4))) / (len + ADR));
	bhdp->endnode = 1;
	unlink(name);
	fd = creat(name, CMODE);
	close(fd);
	fd = open(name, OPENMODE);
	write(fd, (char *)bhdp, NODE);
	close(fd);
	free(bhdp);
}
