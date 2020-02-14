#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "linkedlist.h"
enum sort_or_remove{BY_NAME,BY_ID};
enum menu{ADD=1,VIEW,SORT_BY_NAME,REMOVE_BY_ID,
	        SORT_BY_ID,REMOVE_BY_NAME,EXIT};

bool stringValidation(char *employee_name)
{
	int string_length=strlen(employee_name),indexOfchar=0;
	if(string_length>NAME_LENGTH)//macros
	{
		printf("\nmore than 10 characters!!\n");
		return false;
	}
	while(indexOfchar<string_length)
	{
		if((employee_name[indexOfchar]>='A' && employee_name[indexOfchar]<='Z')||
		   ( employee_name[indexOfchar]>='a' && employee_name[indexOfchar]<='z') ||
			   employee_name[indexOfchar]==' ')
		{
			indexOfchar++;
			continue;
		}
		else
		{
			printf("\nEnter valid string\n");
			return false;
		}
   }
	return true;
}
struct employee* add_employee(struct employee* first_employee) //remove this argument
{
	 //remove int details
	 //remove this array
	struct employee *employee_ptr,*temp_ptr;
	if(!(employee_ptr=(struct employee *)malloc(sizeof(struct employee)))) //malloc validation
	{
		printf("\nmemory not allocated\n");
		return first_employee;
	}

	//getting employee name
	 //change as discussed
	do
	{
		printf("enter the name of the employee  :");
		scanf("%[^\n]s",employee_ptr->employee_name);
	} while(!stringValidation(employee_ptr->employee_name));
	// getting employee id
	do
	{
		printf("enter the id of the employee  :");
		scanf("%5d",&employee_ptr->employee_id); //integer validation required
		if(getchar()!='\n')
		{
			while(getchar()!='\n');
			printf("\nEnter valid id\n");
		}
		else
		{
			break;
		}
	} while(1);

	//getting employee age
	do
	{
		printf("enter the age of the employee  :");
		scanf("%2d",&employee_ptr->employee_age); //integer validation required
		if(getchar()!='\n')
		{
			while(getchar()!='\n');
			printf("\nEnter valid age\n");
		}
		else
		{
			break;
		}
	} while(1);

	//getting employee designation
	do
	{
		printf("enter the designation of the employee  :");
		scanf("%s",employee_ptr->employee_designation);
	} while(!stringValidation(employee_ptr->employee_designation));

	if(!first_employee)
	{
		employee_ptr->next_employee=NULL;
		first_employee=employee_ptr;
	}
	else
	{
		employee_ptr->next_employee=NULL;
		temp_ptr=first_employee;
		while(temp_ptr->next_employee!=NULL)
		{
			temp_ptr=temp_ptr->next_employee;
		}
		temp_ptr->next_employee=employee_ptr;
	}
	return first_employee; //when will it be executed?
}
void display_employee_details(struct employee* first_employee)
{
	if(first_employee!=NULL)
	{
	struct employee *display_ptr;
	display_ptr=first_employee;
	while(display_ptr!=NULL)
	{
	 	printf("NAME        :%s\n", display_ptr->employee_name);
	 	printf("ID          :%d\n", display_ptr->employee_id);
	 	printf("AGE         :%d\n", display_ptr->employee_age);
	 	printf("DESIGNATION :%s\n\n", display_ptr->employee_designation);
		display_ptr=display_ptr->next_employee;
	}
  }
	else
	{
		printf("\nadd any employee to view their details\n");
	}
}

struct employee* removeEmployee(struct employee * first_employee,
	                              int sorting_type)
{
	char name[NAME_LENGTH];
	int ID;
	if(first_employee)
	{
		struct employee *current=first_employee,
		                *next=current->next_employee,*previous=NULL;
		if(!sorting_type)
		{
			do
			{
				printf("Enter the name of the employee  :");
				scanf("%s",name);
			} while(!stringValidation(name));
		}
		else
		{
			do
			{
				printf("Enter the id of the employee  :");
				scanf("%5d",&ID);
				if(getchar()!='\n')
				{
					while(getchar()!='\n');
					printf("\nEnter valid id\n");
				}
				else
				{
					break;
				}
			} while(1);
		}
		while(1)
		{
			if(!current)
			{
				printf("\nEmployee is not present\n");
			}
			else
			{
				if((!sorting_type)?
				    strcmp(current->employee_name,name)==0 : current->employee_id == ID)
				{
					if(current==first_employee)
					{
						first_employee=next;
						free(current);
						printf("\nEmployee details removed\n");
						return first_employee;
					}
					else
					{
						previous->next_employee=current->next_employee;
						free(current);
						printf("\nEmployee details removed\n");
						return first_employee;
					}
				}
				else
				{
					previous=current;
					current=next;
					next=next->next_employee;
				}
			}
		}
	}
	else
	{
		printf("\nAdd any employee to remove\n");
	}
	return first_employee;
}


struct employee* sort_employee_details(struct employee* first_employee,
	                                     int sorting_type)//remove employee count argument
{
	int number_of_swap;
	struct employee *current,*next,*previous;
	if(first_employee!=NULL && first_employee->next_employee!=NULL)
	{
		do{
		 	current=first_employee;
			next=current->next_employee;
			previous=NULL;
			number_of_swap=0;
			while(next!=NULL)
			{
				if((!sorting_type)?
				     strcmp(current->employee_name,next->employee_name)>0 :
						       current->employee_id > next->employee_id)
					{
						if(current==first_employee)
						{
							first_employee=next;
						}
						else
						{
							previous->next_employee=next;
						}
						current->next_employee = next->next_employee;
						next->next_employee = current;
						previous = next;
						next = current->next_employee;
						number_of_swap+=1;
					}
					else
				 	{
						previous=current;
						current=current->next_employee;
						next=current->next_employee;
					}
			}

		}while(number_of_swap>0);
		printf("\nEmployee details are sorted\n");
	}
	else
	{
		printf("\nsorting needs atleast 2 employees\n");
	}
	return first_employee;
}

int main()
{
	//remove this variable
	bool continuing=true;
	struct employee *first_employee=NULL;
	int choice;
	while(continuing)//remove condition
	{
		printf("1 to add employee\n2 to view employee details\
		        \n3 to sort employee details by name\n4 to remove employee by id\
						\n5 to sort employee details by ID\n6 to remove employee by name\
						\n7 to exit\n");
		do
		{
			printf("enter your choice :");
			scanf("%d",&choice); //integer validation required
			if(getchar()!='\n')
			{
				while(getchar()!='\n');
				printf("\nEnter a valid choice\n");
			}
			else
			{
				break;
			}
		} while(1);

    switch(choice)
		{
			  case ADD:
					if(!(first_employee=add_employee(first_employee)))
					{
						printf("\nadd function is not executed \n");
    			}
					break;
				case VIEW:
					 display_employee_details(first_employee);
					 break;
				case SORT_BY_NAME:
						first_employee=sort_employee_details(first_employee,BY_NAME);//function naming not meaningful
						//need return type
						break;
				case REMOVE_BY_ID:
	 				 	first_employee=removeEmployee(first_employee,BY_ID);
					 	break;
			  case SORT_BY_ID:
						first_employee=sort_employee_details(first_employee,BY_ID);
						break;
				case REMOVE_BY_NAME:
						first_employee=removeEmployee(first_employee,BY_NAME);
						break;
		    case EXIT:
						continuing=false;
						break;
			  default:
						printf("\nenter the right choice\n\n");
						break;
		 }
	  }
		free(first_employee);
		return 0;
}
