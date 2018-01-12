#include "file_spec.h"
#include "data_element.h"

/* file names */
char *dbfiles[] = {
	"CLIENTS",
	"PROJECTS",
	"CONSULTANTS",
	"ASSIGNMENTS",
	(char *) 0
};

/* file contents (terminated by a zero value integer) */
int clients_file[] = {
	CLINET_NO, CLIENT_NAME, ADDRESS, CITY, STATE, ZIP, PHONE, AMT_DUE, 0 };
int projects_file[] = {
	PROJECT_NO, PROJECT_NAME, AMT_EXPENDED, CLIENT_NO, 0 };
int consultants_file[] = {
	CONSULTANT_NO, CONSULTANT_NAME, 0 };
int assignments_file[] = {
	CONSULTANT_NO, PROJECT_NO, RATE, 0 };

int *file_ele[] = {
	clients_file,
	projects_file,
	consultants_file,
	assignments_file,
	(int *) 0
};
