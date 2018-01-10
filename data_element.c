#include "data_element.h"

/* data element lenghts */
int ellen[] = {
	5, 25, 25, 25, 2, 5, 10, 8, 5, 25, 9, 5, 25, 5, 9, 9, 2
};

/* uppercase string names for data elements */
char *denames[] = {
	"CLIENT_NO",
	"CLIENT_NAME",
	"ADDRESS",
	"CITY",
	"STATE",
	"ZIP",
	"PHONE",
	"AMT_DUE",
	"PROJECT_NO",
	"PROJECT_NAME",
	"AMT_EXPENDED",
	"CONSULTANT_NO",
	"CONSULTANT_NAME",
	"RATE",
	"PAYMENT",
	"EXPENSE",
	"HOURS",
	(char *) 0
};

/* data element display characteristics */
char eltype[] = "ZAAAANNCZACZACCCN";
char *elmask[] = {
	"_____",
	"_________________________",
	"_________________________",
	"_________________________",
	"__",
	"_____",
	"(___)___-____",
	"$______.__",
	"_____",
	"_________________________",
	"$_______.__",
	"_____",
	"_________________________",
	"$___.__",_
	"$_______.__",
	"$_______.__",
	"__"
};
