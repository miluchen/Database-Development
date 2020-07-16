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
	/* Each entry in keyspace is a key + RPTR pair with length of (KLEN + ADR).
	 * For a nonleaf node, RPTR indicates the associated b-tree node.
	 * For aleaf node, RPTR indicates the associated record position. */
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
RPTR locate(int tree, char *key) {
	int idx, fnd = FALSE;
	RPTR node, ans;
	char *adr;

	trx = tree;
	node = bheader[trx].rootnode;
	if (node) {
		read_node(node, &trnode);
		fnd = btreescan(&node, key, &adr);
		ans = leaflevel(&node, &adr, &idx);
		if (idx == trnode.keyct + 1) {  // TODO: when does this happen and why?
			idx = 0;
			node = trnode.rtsib;
		}
		currnode[trx] = node;  // TODO: how is it used?
		currkno[trx] = idx;  // TODO: how is it used?
	}
	return fnd ? ans : (RPTR) 0;
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
static int btreescan(RPTR *nodeptr, char *key, char **entryadr) {
	int nonleaf;
	do {
		if (nodescan(key, entryadr)) {
			while (compare_keys(*entryadr, key) == 0) {
				/* keep scanning previous key if they are the same */
				if (scanprev(nodeptr, entryadr) == 0)
					break;
			}
			/* if previous key is not the same with key, scan next */
			if (compare_keys(*entryadr, key))
				scannext(nodeptr, entryadr);
			return TRUE;
		}
		nonleaf = trnode.nonleaf;
		if (nonleaf) {
			/* (*entryadr - ADR) points to the child that may contain key */
			*nodeptr = *((RPTR *)(*entryadr - ADR));
			read_node(*nodeptr, &trnode);
		}
	} while (nonleaf);
	return FALSE;
}

/****** scan to the previous sequential key ******/
/* find the previous key of adr which is in node *p currently */
static RPTR scanprev(RPTR *nodeptr, char **adr) {
	RPTR child;

	if (trnode.nonleaf) {
		*nodeptr = *((RPTR *)(*adr - ADR));
		read_node(*nodeptr, &trnode);
		while (trnode.nonleaf) {
			/* read the last node in trnode */
			*nodeptr = *((RPTR *)(trnode.keyspace + trnode.keyct * ENTLN - ADR));
			read_node(*nodeptr, &trnode);
		}
		/* get the last key in trnode */
		*adr = trnode.keyspace + (trnode.keyct - 1) * ENTLN;
		return *((RPTR *)(*adr + KLEN));
	}
	while (-1) {
		if (trnode.keyspace != *adr) {
			*adr -= ENTLN;
			/* return the RPTR of the record in data file */
			return fileaddr(*nodeptr, *adr);
		}
		if (trnode.prntnode == 0 || trnode.lfsib == 0)
			break;
		child = *nodeptr;
		*nodeptr = trnode.prntnode;
		read_node(*nodeptr, &trnode);
		*adr = trnode.keyspace;
		/* find where this child is in parent node */
		while (*((RPTR *)(*adr - ADR)) != child)
			*adr += ENTLN;
	}
	return (RPTR) 0;
}

/****** scan to the next sequential key ******/
static RPTR scannext(RPTR *nodeptr, char **adr) {
	RPTR child;

	if (trnode.nonleaf) {
		*nodeptr = *((RPTR *)(*adr + KLEN));
		read_node(*nodeptr, &trnode);
		while (trnode.nonleaf) {
			/* get the first node in trnode */
			*nodeptr = trnode.key0;
			read_node(*nodeptr, &trnode);
		}
		/* get the first key in trnode */
		*adr = trnode.keyspace;
		return *((RPTR *)(*adr + KLEN));
	}
	*adr += ENTLN;
	while (-1) {
		if ((trnode.keyspace + trnode.keyct * ENTLN) != *adr)
			return fileaddr(*nodeptr, *adr);
		if (trnode.prntnode == 0 || trnode.rtsib == 0)
			break;
		child = *nodeptr;
		*nodeptr = trnode.prntnode;
		read_node(*nodeptr, &trnode);
		*adr = trnode.keyspace;
		while (*((RPTR *)(*adr - ADR)) != child)
			*adr += ENTLN;
	}
	return (RPTR) 0;
}

/****** compute current file address ******/
static RPTR fileaddr(RPTR node, char *adr) {
	RPTR rec, tmp;
	int idx;

	tmp = node;
	rec = leaflevel(&tmp, &adr, &idx);
	read_node(node, &trnode);
	return rec;
}

/****** navigate down to leaf level ******/
/* This function returns the associated RPTR of the file record. */
static RPTR leaflevel(RPTR *nodeptr, char **adr, int *idx) {
	if (trnode.nonleaf == FALSE) {
		*idx = (*adr - trnode.keyspace) / ENTLN + 1;
		return *((RPTR *)(*adr + KLEN));
	}
	*idx = 0;
	*nodeptr = *((RPTR *)(*adr + KLEN));
	read_node(*nodeptr, &trnode);
	*adr = trnode.keyspace;  // TODO: this seems to be not useful?
	while (trnode.nonleaf) {
		*nodeptr = trnode.key0;
		read_node(*nodeptr, &trnode);
	}
	return trnode.key0;
}

/****** search node ******/
static int nodescan(char *keyvalue, char **entryadr) {
	int i, result;

	*entryadr = trnode.keyspace;
	for (i=0; i<trnode.keyct; i++) {
		result = compare_keys(keyvalue, *entryadr);
		if (result == 0)
			return TRUE;
		if (result < 0)
			return FALSE;
		*entryadr += ENTLN;
	}
	return FALSE;
}

/****** compare keys ******/
/* Returns 0, i.e. FALSE, if "a" and "b" are identical */
static int compare_keys(char *a, char *b) {
	int len = KLEN, cm;

	while (len--) {
		if ((cm = (int)*a++ - (int)*b++) != 0)
			break;
	}
	return cm;
}
