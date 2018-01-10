/* ******************************
 * Date element type code:
 * A	= alphanumeric
 * C	= currency
 * Z	= numeric, zero-filled
 * N	= numeric, space-filled
 * D	= date
 ********************************/

#define CLIENT_NO 1
#define CLIENT_NAME 2
#define ADDRESS 3
#define CITY 4
#define STATE 5
#define ZIP 6
#define PHONE 7
#define AMT_DUE 8
#define PROJECT_NO 9
#define PROJECT_NAME 10
#define AMT_EXPENDED 11
#define CONSULTANT_NO 12
#define CONSULTANT_NAME 13
#define RATE 14
#define PAYMENT 15
#define EXPENSE 16
#define HOURS 17

extern int ellen[];
extern char *denames[];
extern char eltype[];
extern char *elmask[];
