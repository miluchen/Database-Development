; --------- cbs.sch	schema for consultant's billing system

#schema CBS

; ------ data element dictionary

#dictionary
	CLIENT_NO,		 Z, 5,  "_____"
	CLIENT_NAME,	 A, 25, "_________________________"
	ADDRESS,		 A, 25, "_________________________"
	CITY,			 A, 25, "_________________________"
	STATE,			 A, 2,  "__"
	ZIP,			 N, 5,  "_____"
	PHONE,			 N, 10, "(___)___-____"
	AMT_DUE,		 C, 8,  "$______.__"
	PROJECT_NO,		 Z, 5,  "_____"
	PROJECT_NAME,	 A, 25, "_________________________"
	AMT_EXPENDED,	 C, 9,  "$_______.__"
	CONSULTANT_NO,	 Z, 5,  "_____"
	CONSULTANT_NAME, A, 25, "_________________________"
	RATE,			 C, 5,  "_____"
	PAYMENT,		 C, 9,  "$_______.__"
	EXPENSE,		 C, 9,  "$_______.__"
	HOURS,			 N, 2,  "__"
	DATE_PAID,		 D, 6,  "__/__/__"
#end dictionary

; ---- file specifications

#file CLIENTS
	CLIENT_NO
	CLIENT_NAME
	ADDRESS
	CITY
	STATE
	ZIP
	PHONE
	AMT_DUE
	DATE_PAID
#end file

#file PROJECTS
	PROJECT_NO
	PROJECT_NAME
	AMT_EXPENDED
	CLIENT_NO
#end file

#file CONSULTANTS
	CONSULTANT_NO
	CONSULTANT_NAME
#end file

#file ASSIGNMENTS
	CONSULTANT_NO
	PROJECT_NO
	RATE
#end file

; ----- index specifications

#key CLIENTS	 CLIENT_NO
#key PROJECTS	 PROJECT_NO
#key CONSULTANTS CONSULTANT_NO
#key ASSIGNMENTS CONSULTANT_NO, PROJECT_NO
#key ASSIGNMENTS CONSULTANT_NO
#key ASSIGNMENTS PROJECT_NO

#end schema CBS
