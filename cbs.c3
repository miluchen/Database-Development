
/* ---------- CBS ---------- */


int ellen[] = {
	5,25,25,25,2,5,10,8,5,25,9,5,25,5,9,9,2,6
};


int f_clients[] = {
	CLIENT_NO,
	CLIENT_NAME,
	ADDRESS,
	CITY,
	STATE,
	ZIP,
	PHONE,
	AMT_DUE,
	DATE_PAID,
	0
};

int f_projects[] = {
	PROJECT_NO,
	PROJECT_NAME,
	AMT_EXPENDED,
	CLIENT_NO,
	0
};

int f_consultants[] = {
	CONSULTANT_NO,
	CONSULTANT_NAME,
	0
};

int f_assignments[] = {
	CONSULTANT_NO,
	PROJECT_NO,
	RATE,
	0
};

int *file_ele[] = {
	f_clients,
	f_projects,
	f_consultants,
	f_assignments,
	0
};

int x1_clients[] = {
	CLIENT_NO,
	0
};

int *x_clients[] = {
	x1_clients,
	0
};

int x1_projects[] = {
	PROJECT_NO,
	0
};

int *x_projects[] = {
	x1_projects,
	0
};

int x1_consultants[] = {
	CONSULTANT_NO,
	0
};

int *x_consultants[] = {
	x1_consultants,
	0
};

int x1_assignments[] = {
	CONSULTANT_NO,
	PROJECT_NO,
	0
};

int x2_assignments[] = {
	CONSULTANT_NO,
	0
};

int x3_assignments[] = {
	PROJECT_NO,
	0
};

int *x_assignments[] = {
	x1_assignments,
	x2_assignments,
	x3_assignments,
	0
};

int **index_ele[] = {
	x_clients,
	x_projects,
	x_consultants,
	x_assignments,
	0
};
