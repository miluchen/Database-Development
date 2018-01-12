# Database-Development

This project is based on "C DATA BASE DEVELOPMENT" by Al Stevens.

Problem definition:
1. It's a consultant's billing system, which allows a consulting firm to track labor charges and expenses against clients' projects and to prepare invoices.
2. The Consultant's Billing System (CBS) has a data base that records time and expense charges against projects for clients.
3. It prepares invoices for labor hours and expenses.
4. It computes labor charges from the hourly rates of consultants assigned to projects.
5. You can post expenses directly.

Design details:
1. The data base have four files: a CONSULTANTS file, a CLIENTS file, a PROJECTS file, and an ASSIGNMENTS file.

CBS tables (elements in parenthesis are key indices):
	CLIENTS:	 (client_no), client_name, address, city, state, phone, amt_due, date_paid
	PROJECTS:	 (project_no), project_name, amt_expended, client_no
	CONSULTANTS: (consultant_no), consultant_name
	ASSIGNMENTS: (consultant_no, project_no), rate
