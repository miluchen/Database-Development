/********************** database.c ***********************/

#include <stdio.h>
#include "cdata.h"
#include "keys.h"

int db_opened = FALSE;	/* data base opened indicator	*/
int curr_fd[MXFILS];	/* current file descriptor		*/

char dbpath[64];

/****** open the data base ******/
/* char *path: path where the data base is */
/* int *fl: file list */
void db_open(char *path, int *fl) {
	char fnm[64];
	int i;

	if (!db_opened) {
		for (i = 0; i < MXFILS; i++)
			curr_fd[i] = -1;
		dp_opened = TRUE;
	}

	strcpy(dbpath, path);
	while (*fl != -1) {
		sprintf(fnm, "%s%s.dat", path, dbfiles[*fl]);
		curr_fd[*fl] = file_open(fnm);
		init_index(path, *fl);
		if ((bfs[*fl] = malloc(rlen(*fl))) == NULL) {
			errno = D_OM;
			dberror();
		}
		fl++;
	}
}
