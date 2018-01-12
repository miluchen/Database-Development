#include "index_spec.h"

int clients_index_1 [] {
	CLINET_NO,
	0
};

int *clients_index [] {
	clients_index_1,
	(int *) 0
};

int projects_index_1 [] {
	PROJECT_NO,
	0
};

int *projects_index [] {
	projects_index_1,
	(int *) 0
};

int consultants_index_1 [] {
	CONSULTANT_NO,
	0
};

int *consultants_index [] {
	consultants_index_1,
	(int *) 0
};

int assignments_index_1 [] {
	CONSULTANT_NO,
	PROJECT_NO,
	0
};

int assignments_index_2 [] {
	CONSULTANT_NO,
	0
};

int assignments_index_3 [] {
	PROJECT_NO,
	0
};

int *assignments_index [] {
	assignments_index_1,
	assignments_index_2,
	assignments_index_3,
	(int *) 0
};

int **index_ele [] {
	clients_index,
	projects_index,
	consultants_index,
	assignments_index,
	(int **) 0
};
