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
