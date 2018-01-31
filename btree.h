#ifndef _BTREE_H_
#define _BTREE_H_

#include "cdata.h"
int btree_init(char *ndx_name);
int btree_close(int tree);
void build_b(char *name, int len);
RPTR locate(int tree, char *k);
int deletekey(int tree, char *x, RPTR ad);
int insertkey(int tree, char *x, RPTR ad, int unique);
RPTR nextkey(int tree);
RPTR prevkey(int tree);
RPTR firstkey(int tree);
RPTR lastkey(int tree);
RPTR currkey(int tree);
void keyval(int tree, char *ky);

#endif
