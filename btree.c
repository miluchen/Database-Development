/*
 * This file contains functions that manage the data base B-tree index files
 * The implemented B-tree is B+ tree, which stores a pointer to another disk page containing
 * the satellite information for a given key in the leaves and stores only keys and child
 * pointers in the internal nodes. Thus, it maximizes the branching factor of the internal nodes.
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
	/* a nonleaf node contains key + RPTR pairs, where RPTR indicates the associated b-tree node
	 * a leaf node contains key + RPTR, where RPTR indicates the associated record position */
	char keyspace[NODE - ((sizeof(int) * 2) + (ADR * 4))];
	char spil[MXKEYLEN];	/* for insertion excess */
} BTREE;

/****** the structure of the btree header node ******/
typedef struct treehdr {
	RPTR rootnode;	/* root node number			*/
	int keylength;  /* length of the key        */
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
/* This function initializes processing for an existing B-tree index file.
 * It returns an integer that is used for subsequent calls to the B-tree management functions. */
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
	if (bheader[trx].locked) {  // TODO: race condition here!!!
		close(handle[trx]);
		handle[trx] = 0;
		printf("Index file is locked: %s\n", ndx_name);
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
/* This function must be called. Otherwise, subsequent attempts to open the B-tree will
 * fail because it's in "locked" state. */
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
/* This functions estabilishes a new B-tree. The "len" is the length of the key value that
 * will be indexed by the B-tree */
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

/****** locate key in the b-tree ******
 * The "tree" parameter is the integer returned by bree_init.
 * 
 * If the key value is in the B-tree, the RPTR value associated is returned.
 * If there are multiple identical key values in the tree, the first is returned.
 * If the key value is not in the tree, 0 is returned.
 */
RPTR locate(int tree, char *k) {
	int i, fnd = FALSE;
	RPTR t, ad;
	char *a;

	trx = tree;
	t = bheader[trx].rootnode;
	if (t) {
		read_node(t, &trnode);
		fnd = btreescan(&t, k, &a);
		ad = leaflevel(&t, &a, &i);
		if (i == trnode.keyct + 1) {  // TODO: when does this happen and why?
			i = 0;
			t = trnode.rtsib;
		}
		currnode[trx] = t;  // TODO: how is it used?
		currkno[trx] = i;  // TODO: how is it used?
	}
	return fnd ? ad : (RPTR) 0;
}

/****** read a btree node ******/
static void read_node(RPTR nd, BTREE *bf) {
	bseek(nd);
	read(handle[trx], (char *)bf, NODE);
}

/****** seek to the b-tree node ******/
static void bseek(RPTR nd) {
	if (lseek(handle[trx], (long)(NODE + ((nd-1) * NODE)), SEEK_SET) == ERROR) {
		errno = D_IOERR;
		dberror();
	}
}

/****** search tree ******/
static int btreescan(RPTR *t, char *k, char **a) {
	int nl;
	do {
		if (nodescan(k, a)) {
			while (compare_keys(*a, k) == FALSE) {
				if (scanprev(t, a) == 0)
					break;
			}
			if (compare_keys(*a, k))
				scannext(t, a);
			return TRUE;
		}

	} while (nl);
	return FALSE;
}

/****** scan to the previous sequential key ******/
static RPTR scanprev(RPTR *p, char **a) {
	RPTR cn;

	if (trnode.nonleaf) {
		*p = *((RPTR *)(*a - ADR));
		read_node(*p, &trnode);
		while (trnode.nonleaf) {
			/* read the last node in trnode */
			*p = *((RPTR *)(trnode.keyspace + trnode.keyct * ENTLN - ADR));
			read_node(*p, &trnode);
		}
		*a = trnode.keyspace + (trnode.keyct - 1) * ENTLN;
		return *((RPTR *)(*a + KLEN));
	}
	while (-1) {
		if (trnode.keyspace != *a) {
			*a -= ENTLN;
			return fileaddr(*p, *a);  // TODO: ???
		}

	}
}

/****** compute current file address ******/
static RPTR fileaddr(RPTR t, char *a) {
	RPTR cn, ti;
	int i;

	ti = t;
	cn = leaflevel(&ti, &a, &i);
	read_node(t, &trnode);
	return cn;
}

/****** navigate down to leaf level ******/
/* This function returns the associated RPTR of the file record. */
static RPTR leaflevel(RPTR *t, char **a, int *p) {
	if (trnode.nonleaf == FALSE) {
		*p = (*a - trnode.keyspace) / ENTLN + 1;
		return *((RPTR *)(*a + KLEN));
	}
	*p = 0;
	*t = *((RPTR *)(*a + KLEN));
	read_node(*t, &trnode);
	*a = trnode.keyspace;  // TODO: this seems to be not useful?
	while (trnode.nonleaf) {
		*t = trnode.key0;
		read_node(*t, &trnode);
	}
	return trnode.key0;
}

/****** search node ******/
static int nodescan(char *keyvalue, char **nodeadr) {
	int i, result;

	*nodeadr = trnode.keyspace;
	for (i=0; i<trnode.keyct; i++) {
		result = compare_keys(keyvalue, *nodeadr);
		if (result == FALSE)
			return TRUE;
		if (result < 0)
			return FALSE;
		*nodeadr += ENTLN;
	}
	return FALSE;
}

/****** compare keys ******/
static int compare_keys(char *a, char *b) {
	int len = KLEN, cm;

	while (len--) {
		if ((cm = (int)*a++ - (int)*b++) != 0)
			break;
	}
	return cm;
}