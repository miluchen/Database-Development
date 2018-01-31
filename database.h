#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "cdata.h"

void db_open(char *path, int *fl);
int add_rcd(int f, char *bf);
int find_rcd(int f, int k, char *key, char *bf);
int verify_rcd(int f, int k, char *key);
int first_rcd(int f, int k, char *bf);
int last_rcd(int f, int k, char *bf);
int next_rcd(int f, int k, char *bf);
int prev_rcd(int f, int k, char *bf);
int rtn_rcd(int f, char *bf);
int del_rcd(int f);	/* delete the current record from the file */
int curr_rcd(int f, int k, char *bf);
int seqrcd(int f, char *bf);
void db_cls();
void dberror();
int rlen(int f);
void init_rcd(int f, char *bf);
void clrrcd(char *bf, int *els);
void rcd_fill(char *s, char *d, int *slist, int *dlist);
int epos(int el, int *list);
void build_index(char *path, int f);
int add_indexes(int f, char *bf, RPTR ad);
void error_message(char *s);
void clear_notice();
void post_notice(char *s);
void mov_mem(char *s, char *d, int l);
int filename(char *fn);

#endif
