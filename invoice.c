/*
 * produce invoices from the clients file
 */

#include <stdio.h>
#include <stdlib.h>
#include "cdata.h"
#include "cbs.c1"

struct clients cl;

int main() {
	static int fl[] = {CLIENTS, -1};

	db_open("", fl);
	while (TRUE) {
		if (next_rcd(CLIENTS, 1, &cl) == ERROR)
			break;
		printf("\n\n\nInvoice for Services Rendered\n");
		printf("\n%s", cl.client_name);
		printf("\n%s", cl.address);
		printf("\n%s, %s %s", cl.city, cl.state, cl.zip);
		printf("\n\nAmount Due: $%10.2f\n", atof(cl.amt_due) / 100);
	}
	db_cls();
}
