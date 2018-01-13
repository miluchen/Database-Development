/******************************* 
 * 1. file specifications
 * 2. file contents
 * 3. record structure definition
 *******************************/

#ifndef _FILE_SPEC_H_
#define _FILE_SPEC_H_

#define CLIENTS 0
#define PROJECTS 1
#define CONSULTANTS 2
#define ASSIGNMENTS 3

extern char *dbfiles[];
extern clients_file[];
extern projects_file[];
extern consultants_file[];
extern assignments_file[];
extern int *file_ele[];

/* record structure, with zero termination */
struct clients {
	char client_no[6];
	char client_name[26];
	char address[26];
	char city[26];
	char state[3];
	char zip[6];
	char phone[11];
	char amt_due[9];
};

struct projects {
	char project_no[6];
	char project_name[26];
	char amt_expended[10];
	char client_no[6];
};

struct consultants {
	char consultant_no[6];
	char consultant_name[26];
};

struct assignments {
	char consultant_no[6];
	char project_no[6];
	char rate[6];
};

#endif
