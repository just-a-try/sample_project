#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include"EmployeeDetails.h"	//why EmployeeDetails"1"? // name changed
#define NAME_LENGTH 10   //Naming to be more specific ///naming changed
enum menu{ADD=1,VIEW,SORT_BY_NAME,REMOVE_BY_ID,
	            SORT_BY_ID,REMOVE_BY_NAME,EXIT};
bool string_validation(char *employee_name)
{
	int string_length=strlen(employee_name),indexOfchar=0;
	if(string_length>NAME_LENGTH)
	{
		printf("more than 10 characters!!\n");
		return false;
	}
	while(indexOfchar<string_length)
		{
			if((employee_name[indexOfchar]>='A' && employee_name[indexOfchar]<='Z')||
			( employee_name[indexOfchar]>='a' && employee_name[indexOfchar]<='z') || employee_name[indexOfchar]==' ')
			{
				indexOfchar++;
				continue;
			}
			else
			{
				printf("Enter the valid string\n");
				return false;
			}
	   }
		 return true;
}

struct employee* add_employee(struct employee *employee_ptr,
	                            int *employee_count)
{
	bool validation=false;
	int name_lenght,indexOfchar=0;
	if(employee_ptr==NULL)
		{
				employee_ptr=(struct employee *)malloc(1*sizeof(struct employee));
		}
	else
	{
		employee_ptr=(struct employee *)realloc(employee_ptr,
			           (*employee_count+1)*sizeof(struct employee));
		if(employee_ptr==NULL)
		{
			printf("not reallocated\n");
		}
  }
	//getting employee name

	// Change as discussed
		do {
			printf("enter the name of the employee %d :",*employee_count+1);
			scanf("%[^\n]s",employee_ptr[*employee_count].employee_name);
		} while(!string_validation(employee_ptr[*employee_count].employee_name));
		//getting employee id
		do {
			printf("\nenter the id of the employee %d :" ,*employee_count+1);
			scanf("%5d",&employee_ptr[*employee_count].employee_id); //integer validation required
			if(getchar()!='\n')
			{
				while(getchar()!='\n');
				printf("Enter valid id\n");
			}
			else
			{
				break;
			}
		} while(1);

		//getting employee age
		do {
			printf("\nenter the age of the employee %d :",*employee_count+1);
			scanf("%2d",&employee_ptr[*employee_count].employee_age); //integer validation required
			if(getchar()!='\n')
			{
				while(getchar()!='\n');
				printf("Enter valid age\n");
			}
			else
			{
				break;
			}
		} while(1);
		 //age validation for accepting only two digits

		//getting employee designation

	 //change as discussed
		do {
			printf("\nenter the designation of the employee %d :",*employee_count+1);
			scanf("%s",employee_ptr[*employee_count].employee_designation);
		} while(!string_validation(employee_ptr[*employee_count].employee_designation));
	*employee_count+=1;
	return employee_ptr;
}


void display_employee_details(struct employee *employee_ptr)
{
 	int index;
	printf("size of employee =%d\n",sizeof(struct employee));
	printf("size of employee ptr =%d\n",sizeof(employee_ptr));
	// for(index=0;index<employee_count;index++)
	// {
	//  	printf("NAME        :%s\n", employee_ptr[index].employee_name);
	//  	printf("ID          :%d\n", employee_ptr[index].employee_id);
	//  	printf("AGE         :%d\n", employee_ptr[index].employee_age);
	//  	printf("DESIGNATION :%s\n\n", employee_ptr[index].employee_designation);
  // }
}

void sort_byname(struct employee *employee_ptr,int employee_count) //remove the temp argument
{
	int employee_count_i,employee_count_j,char_index=0;
  struct employee temp;
	//bubble sort
	for(employee_count_i=0;
		  employee_count_i<employee_count-1;employee_count_i++)
	{
		for(employee_count_j=employee_count_i+1;
			  employee_count_j<=employee_count-1;employee_count_j++)
		{
			while(employee_ptr[employee_count_i].employee_name[char_index]==
				    employee_ptr[employee_count_j].employee_name[char_index])
			{
				if(employee_ptr[employee_count_i].employee_name[char_index]=='\0' &&
				   employee_ptr[employee_count_j].employee_name[char_index]=='\0')
				{
					break;
				}
				char_index++;
			}
			if(employee_ptr[employee_count_i].employee_name[char_index]>
				 employee_ptr[employee_count_j].employee_name[char_index])
			{
				temp=employee_ptr[employee_count_i];
				 employee_ptr[employee_count_i]=employee_ptr[employee_count_j];
				 employee_ptr[employee_count_j]=temp;
			}
			char_index=0;
		}
	}
//why this is required?
}

void sort_byID(struct employee *employee_ptr,int employee_count)
{
	int employee_count_i,employee_count_j;
	struct employee temp;
	//bubble sort
	for(employee_count_i=0;
		  employee_count_i<employee_count-1;employee_count_i++)
	{
		for(employee_count_j=employee_count_i+1;
			  employee_count_j<=employee_count-1;employee_count_j++)
		{
			if(employee_ptr[employee_count_i].employee_id>
				 employee_ptr[employee_count_j].employee_id)
			{
				 temp=employee_ptr[employee_count_i];
				 employee_ptr[employee_count_i]=employee_ptr[employee_count_j];
				 employee_ptr[employee_count_j]=temp;
			}
		}
	}
 //why this is required?
}

bool remove_employee_byid(struct employee *employee_ptr,int *employee_count) //why deleted entry data not cleared in this function?
{
    int index,remove_id;
    bool id_check=true;
		do {
			printf("Enter the employee id to remove : ");
			scanf("%5d",&remove_id); //validation required
			if(getchar()!='\n')
			{
				while(getchar()!='\n');
				printf("Enter valid id\n");
			}
			else
			{
				break;
			}
		} while(1);
		//add your comments for the logic
	  for(index=0;index<*employee_count;index++) //traverse entire structure
	  {
			if(remove_id==employee_ptr[index].employee_id)//comparing the id
				{
					id_check=false;
					if(index==*employee_count-1)//checking whether its a last element
					{
		     		break;
					}
					else
					{
						while((index+1)!=*employee_count)//moving the structures to replace the deleted element
						{
							employee_ptr[index]=employee_ptr[index+1];
							index++;
						}
						index++;
					}
				}
		}
			if(id_check)
			{
				printf("Given id is not present, enter a valid id\n");
				return false;
			}
			else
			{
				*employee_count-=1;
				return true;
			}
}

bool remove_employee_byname(struct employee *employee_ptr,int *employee_count) //same comments as in remove by id
{
    int index;
    bool name_check=true;
    char removing_name[10];
		do {
			printf("Enter the employee name to remove : ");
			scanf("%s",removing_name);
		} while(!string_validation(removing_name));
		for(index=0;index<*employee_count;index++)
		{
			if(strcmp(removing_name,employee_ptr[index].employee_name)==0)
			{
				name_check=false;
				if(index==*employee_count-1)
				{
			     	break;
				}
				else
				{
	 				while((index+1)!=*employee_count)
					{
						employee_ptr[index]=employee_ptr[index+1];
						index++;
					}
	        index++;
				}

			}
		}
		if(name_check)
		{
			printf("Given name is not present, enter a valid name\n");
			return false;
		}
		else
		{
			*employee_count-=1;
			return true;
		}
}

int main()
{
	struct employee *employee_ptr=NULL; //remove this temp
	bool continuing=true;
	int choice,employee_count=0; //choice spelling
	while(continuing)
	{
		printf("1 to add employee\n2 to view employee details\
		       \n3 to sort employee details by name\n4 to remove employee by id\
					 \n5 to sort employee details by ID\n6 to remove employee by name\
					 \n7 to exit\n");
		do
		{
			printf("enter your choice :");
			scanf("%d",&choice);
			if(getchar()!='\n')
			{
				while(getchar()!='\n');
				printf("Enter a valid choice\n");
			}
			else
			{
				break;
			}
		} while(1);
    switch(choice)
		{
			//use enums for switch
			case ADD:
					 //why not validated after return?
					employee_ptr = add_employee(employee_ptr,&employee_count);
					if(!employee_ptr)
					{
						printf("add function not executed\n");
						break;
					}
					break;
		  case VIEW:
		    	if(employee_count!=0 ) //why restrict only to maximum of 3? // sorting need to data
		    	{
						display_employee_details(employee_ptr);
          }
		      else
					{
		        printf("Add any employee to view their details\n\n");
					}
					break;
		  case SORT_BY_NAME:
		    	if(employee_count!=0 && employee_count>1)//why restrict only to maximum of 3? //// sorting need two data
		    	{
            sort_byname(employee_ptr,employee_count); //why this "=="
						printf("details are sorted\n");
		      }
	        else
					{
	        	printf(" Add any employees to sort their details\n\n");
					}
					break;
		   case REMOVE_BY_ID:
		    	if(employee_count!=0)
		    	{
				    if(remove_employee_byid(employee_ptr,&employee_count))
						{
							printf("employee details removed\n");
							if(employee_count==0)
							{
								free(employee_ptr);
								employee_ptr=NULL;
							}
							else
							{
								employee_ptr=(struct employee *)realloc(employee_ptr,
									           (employee_count)*sizeof(struct employee));//realloc validation and display message accordingly
								if(employee_ptr==NULL)
								{
									printf("reallocation is not done\n");
									return false;
								}
							}
							 //remove this function
						}
		      }
		    	else
		    	{
		    		printf("First add any employee to remove their details\n\n");
					}
					break;
			 	case SORT_BY_ID:
			     if(employee_count!=0 && employee_count>1)
		    		{
							sort_byID(employee_ptr,employee_count);
						}
		    		else
		    		{
		    			printf("First add any employee to sort their details\n\n");
						}
						break;
		    case REMOVE_BY_NAME:
			    if(employee_count!=0)
		    	{
				    if(remove_employee_byname(employee_ptr,&employee_count)) //same as remove by id
						{
							printf("employee details removed\n");
						}
	        }
		    	else
		    	{
		    		printf("First add any employee to remove their details\n\n");
					}
					break;
				case EXIT:
					continuing=false;
					break;
				default:
					printf("enter the right choice\n\n");
					break;
		}

	}
	if(employee_ptr!=NULL)
	{
		printf("freed\n");
		free(employee_ptr); //study about free function and modify if any
	}
	return 0;
}
