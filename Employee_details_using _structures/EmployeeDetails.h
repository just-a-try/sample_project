struct employee
{
	int employee_id;
	char employee_name[10];
	int employee_age;
	char employee_designation[10];
};
bool string_validation(char *);
struct employee* add_employee(struct employee *,int*);
void display_employee_details(struct employee *);
void sort_byname(struct employee *, int);
bool remove_employee_byid(struct employee *,int*);
void sort_byID(struct employee *, int );
bool remove_employee_byname(struct employee *,int*);
