#define NAME_LENGTH 10

struct employee
{
	int employee_id;
	char employee_name[NAME_LENGTH]; //use macros
	int employee_age;
	char employee_designation[NAME_LENGTH]; //use macros
	struct employee *next_employee;
};

 //remove this global and declare this inside main and pass the head to all functions
struct employee* addEmployee(struct employee *);
void displayEmployeeDetails(struct employee*);
struct employee* sortEmployeeDetails(struct employee*,int);
 //Combine to one function
bool stringValidation(char *);
struct employee* removeEmployee(struct employee*,int);
