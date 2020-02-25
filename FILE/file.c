#include"file.h"

/* @authhor: Subhash
 * @function: to get the number and validate
 * @param char pointer validation_msg: message to be displayed
 * @return: returns a interger type number after validation
 */
 int getNumberAndValidate(char *validation_msg)
 {
     int number = ZERO;

     while(!number)
     {
         scanf("%d", &number);

         if(getchar() != '\n')
         {
             while(getchar() != '\n');
             printf("%s\n", validation_msg);
         }
     }

     return number;
 }

/* @author: Subhash
 * @function: to find and replace a string
 * @param char pointer file_name: string of the file name
 * @return: returns true if function is succeded or false if function failed
*/
bool findAndReplace(char *file_name)
{
    int word_found_count, temp_index, old_index = ZERO, new_index,
        file_index = ZERO, replace_index = ZERO, old_len, new_len, file_size, replace_size;
    char *file_content, *replaced_content, *old_word, *new_word;
    FILE *fp;
    file_size = fileSize(file_name);
    printf("filesize %d\n", file_size);
    printf("Enter the word to be replaced\n");
    scanf("%ms", &old_word);

    if((word_found_count = findWord(file_name, old_word)) > MINIMUM_WORD_FOUND)
    {
        old_len = strlen(old_word);
        printf("Enter the new word\n");
        scanf("%ms", &new_word);
        new_len = strlen(new_word);

        if(!(file_content = getContentOfThefile(file_name)))
        {
            printf("Error in function getContentOfThefile\n");
            free(old_word);
            free(new_word);
            return false;
        }

        if(old_len > new_len)
        {
            replace_size = file_size - (word_found_count * (old_len - new_len));
        }
        else
        {
            replace_size = file_size + (word_found_count * (new_len - old_len));
        }

        if(!(replaced_content = (char*)malloc(replace_size * sizeof(char))))
        {
            printf("memory not allocated\n");
            free(file_content);
            free(old_word);
            free(new_word);
            return false;
        }

        while(file_index < file_size)
        {
            if(old_word[old_index] == file_content[file_index])
            {
                old_index++;
                if(old_index == old_len)
                {
                    for(new_index=ZERO;new_index<new_len;new_index++)
                    {
                        replaced_content[replace_index]=new_word[new_index];
                        replace_index++;
                    }
                    old_index = ZERO;
                }
                else
                {
                    for(temp_index=file_index+1;temp_index<(file_index+old_len);
                                                temp_index++)
                    {
                        if(old_word[old_index] == file_content[temp_index])
                        {
                            old_index++;
                            if(old_index == old_len)
                            {
                                for(new_index=ZERO;new_index<new_len;new_index++)
                                {
                                    replaced_content[replace_index]=new_word[new_index];
                                    replace_index++;
                                }
                                file_index = temp_index+1;
                            }
                        }
                        else
                        {
                            old_index = ZERO;
                            replaced_content[replace_index] = file_content[file_index];
                            replace_index++;
                            break;
                        }
                    }
                }
            }
            else
            {
                replaced_content[replace_index] = file_content[file_index];
                replace_index++;
            }

            file_index++;
        }

        if(!(fp=fopen(file_name, "wb")))
        {
            printf("given file is not present\n");
            free(file_content);
            free(old_word);
            free(new_word);
            free(replaced_content);
            return false;
        }

        if(!fwrite(replaced_content, replace_size, NMEMB, fp))
        {
            printf("fread is not excecuted successfully\n");

            if(fclose(fp) == EOF)
            {
                printf("The file is not closed successfully\n");
            }

            free(file_content);
            free(old_word);
            free(new_word);
            free(replaced_content);
            return false;
        }

        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
            free(file_content);
            free(old_word);
            free(new_word);
            free(replaced_content);
            return false;
        }

        free(file_content);
        free(old_word);
        free(new_word);
        free(replaced_content);
    }
    else
    {
        free(old_word);
        printf("Given word is not present\n");
    }
    return true;
}

/* @authhor: Subhash
 * @function: to get the size of the file
 * @param char pointer file_name: string of the file name
 * @return: returns the interger type file size or FAILED macro if function fails
 */
int fileSize(char *file_name)
{
    FILE *fp;
    int file_size;

    if(!(fp=fopen(file_name, "rb")))
    {
        printf("No content present in the file\n");
        return ZERO;
    }

    if(fseek(fp, SEEK_OFFSET, SEEK_END))
    {
        printf("fseek is not excecuted successfully\n");
        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
        }
        return FAILED;
    }

    file_size = ftell(fp);

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
        return FAILED;
    }

    return file_size;
}


 /* @authhor: Subhash
  * @function: to get the content of the file
  * @param char pointer file_name: string of the file name
  * @return: returns the character pointer of the file content
  */
 char* getContentOfThefile(char *file_name)
 {
     FILE *fp;
     char *file_content;
     int file_size;
     file_size=fileSize(file_name);

     if(!(fp=fopen(file_name, "rb")))
     {
         printf("content is not present\n");
         if(fclose(fp) == EOF)
         {
             printf("The file is not closed successfully\n");
         }
         return NULL;
     }

     if(!(file_content = (char*)malloc(file_size * sizeof(char))))
     {
         printf("memory not allocated\n");
         if(fclose(fp) == EOF)
         {
             printf("The file is not closed successfully\n");
         }
         return NULL;
     }

     if(!fread(file_content, file_size, NMEMB, fp))
     {
        printf("fread is not excecuted successfully\n");
        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
        }
        free(file_content);
        return NULL;
     }

     if(fclose(fp) == EOF)
     {
         printf("The file is not closed successfully\n");
         free(file_content);
         return NULL;
     }

     return file_content;
 }

/* @author: Subhash
   @function: to find the number of occurrences of the given words
   @param char pointer file_name: string of the file name
   @return: returns count of the word found or return FAILED if function failed
*/
 int findWord(char *file_name, char *word)
 {
     char *file_content;
     int word_length, file_size, index_of_file = ZERO, index_of_word = ZERO, count = ZERO;
     word_length = strlen(word);
     file_size = fileSize(file_name);

     if(!(file_content = getContentOfThefile(file_name)))
     {
         printf("Error in function getContentOfThefile\n");
         return FAILED;
     }

     while(index_of_file < file_size)
     {
         if(word[index_of_word] == file_content[index_of_file])
         {
             index_of_word++;
             if(word_length == index_of_word)
             {
                 count += 1;
                 index_of_word = ZERO;
             }
         }
         else
         {
             index_of_word = ZERO;
         }

         index_of_file++;
     }

     free(file_content);
     return count;
 }

/* @authhor: Subhash
 * @function: to print the content of the given file
 * @param char pointer file_name: string of the file name
 * @return: returns true if function is succeded or false if failed
 */
bool displayFile(char *file_name)
{
    char *file_content;
    int file_size, file_index = ZERO;
    file_size = fileSize(file_name);

    if(!(file_content = getContentOfThefile(file_name)))
    {
        printf("Error in function getContentOfThefile\n");
        return false;
    }
    printf("File contents are \n");

    while(file_index < file_size)
    {
        printf("%c", file_content[file_index]);
        file_index++;
    }

    free(file_content);
    return true;
}

/* @authhor: Subhash
 * @function: to append any string in the file at given place
 * @param char pointer file_name: string of the file name
 * @return: returns boolean true if appended successfully or false when it fails
 */
 bool insert(char *file_name)
 {
      FILE *fp;
      int position=ZERO,file_size, choice, insert_len, insert_index = ZERO, temp_index, total_index = ZERO;
      char *temp_str, *insert_string, *total_str;
      file_size = fileSize(file_name);

      if(!file_size)
      {
            if(!(fp = fopen(file_name, "wb")))
            {
                printf("given file is not present\n");
                return false;
            }

            printf("Type any content\n");
            scanf("%m[^\n]", &insert_string);
            insert_len = strlen(insert_string);

            if(fwrite(insert_string, insert_len, NMEMB, fp) == EOF)
            {
                printf("fwrite is not excecuted successfully\n");
                if(fclose(fp) == EOF)
                {
                    printf("The file is not closed successfully\n");
                }
                free(insert_string);
                return false;
            }
      }
      else
      {
              printf("\n1-- to insert at the beginning\n2-- to insert at the end\
                      \n3 -- to insert at any position\n");
              choice=getNumberAndValidate("enter the right choice ");
              printf("enter the string to be inserted\n");
              scanf("%m[^\n]", &insert_string);
              insert_len = strlen(insert_string);

              if(!(fp=fopen(file_name, "rb+")))
              {
                  printf("given file is not present\n");
                  return false;
              }

              if(!(temp_str = (char*)malloc((insert_len + file_size)*sizeof(char))))
              {
                  printf("memory not allocated\n");
                  free(insert_string);
                  return NULL;
              }

              if(!(total_str = getContentOfThefile(file_name)))
              {
                  printf("Error in function getContentOfThefile\n");
                  free(temp_str);
                  free(insert_string);
                  return false;
              }

              switch(choice)
              {
                  case FRONT:
                      for(temp_index = ZERO;temp_index < (file_size + insert_len);temp_index++)
                      {
                          if(temp_index < insert_len)
                          {
                              temp_str[temp_index] = insert_string[insert_index];
                              insert_index++;
                          }
                          else
                          {
                              temp_str[temp_index] = total_str[total_index];
                              total_index++;
                          }
                      }

                      if(fwrite(temp_str, (file_size + insert_len), NMEMB, fp) == EOF)
                      {
                          printf("fwrite is not excecuted successfully\n");
                          if(fclose(fp) == EOF)
                          {
                              printf("The file is not closed successfully\n");
                          }
                          free(temp_str);
                          free(insert_string);
                          free(total_str);
                          return false;
                      }
                      break;
                  case END:
                      for(temp_index = ZERO;temp_index < (file_size + insert_len);temp_index++)
                      {
                          if(temp_index < file_size)
                          {
                              temp_str[temp_index] = total_str[total_index];
                              total_index++;
                          }
                          else
                          {
                              temp_str[temp_index] = insert_string[insert_index];
                              insert_index++;
                          }
                      }

                      if(fwrite(temp_str, (file_size + insert_len), NMEMB, fp) == EOF)
                      {
                          printf("fwrite is not excecuted successfully\n");
                          if(fclose(fp) == EOF)
                          {
                              printf("The file is not closed successfully\n");
                          }
                          free(temp_str);
                          free(insert_string);
                          free(total_str);
                          return false;
                      }
                      break;
                  case POSITION:
                      do{
                      printf("Enter the position between 1 to %d\n", file_size);
                      position = getNumberAndValidate("Enter the right position\n");
                      }while(position < ZERO || position > file_size);

                      for(temp_index = ZERO;temp_index < (file_size + insert_len);temp_index++)
                      {
                          if(temp_index < (position -1) || temp_index > (position  + insert_len - 2))
                          {
                              temp_str[temp_index] = total_str[total_index];
                              total_index++;
                          }
                          else
                          {
                              temp_str[temp_index] = insert_string[insert_index];
                              insert_index++;
                          }
                      }

                      if(fwrite(temp_str, (file_size + insert_len), NMEMB, fp) == EOF)
                      {
                          printf("fwrite is not excecuted successfully\n");
                          if(fclose(fp) == EOF)
                          {
                              printf("The file is not closed successfully\n");
                          }
                          free(temp_str);
                          free(insert_string);
                          free(total_str);
                          return false;
                      }
                      break;
                  default:
                      printf("enter the right choice\n");
                      break;
              }
              free(temp_str);
              free(total_str);
      }

      free(insert_string);

      if(fclose(fp) == EOF)
      {
          printf("The file is not closed successfully\n");
          return false;
      }

      return true;
 }

int main()
{
    int choice, count, file_size;
    bool loop_run = true;
    char *file_name, *word;
    printf("Enter the file name : ");
    scanf("%ms", &file_name);
    printf("file name is %s\n", file_name);

    while(loop_run)
    {
        printf("\n          File operation          \n");
        printf("\n1-- File size\n2-- insert words in the file\
                \n3-- Find word in the file\
                \n4-- Find and replace the word in the file\
                \n5-- display the file content\n6-- Exit\n");
        choice=getNumberAndValidate("enter the right choice ");

        switch(choice)
        {
            case SIZE:
                if((file_size = fileSize(file_name)) == FAILED)
                {
                    printf("fileSize function not executed successfully\n");
                }
                else
                {
                    printf("file size is %d", file_size);
                }
                break;
            case INSERT:
                if(!insert(file_name))
                {
                    printf("insert function not executed successfully\n");
                }
                break;
            case FIND:
                if((file_size = fileSize(file_name)))
                {
                    printf("Enter the word to be found\n");
                    scanf("%m[^\n]", &word);
                    if((count = findWord(file_name, word)) == FAILED)
                    {
                        printf("Find word function not executed successfully\n");
                    }
                    else
                    {
                        printf("number of given word present in the file %d\n", count);
                    }
                    free(word);
                }
                break;
            case REPLACE:
                if((file_size = fileSize(file_name)))
                {
                    if(!findAndReplace(file_name))
                    {
                        printf("findAndReplace function is not executed successfully\n");
                    }
                }
                break;
            case DISPLAY:
                if((file_size = fileSize(file_name)))
                {
                    if(!displayFile(file_name))
                    {
                        printf("display function not executed successfully\n");
                    }
                }
                break;
            case EXIT:
                loop_run=false;
                break;
            default:
                printf("enter the right choice\n");
                break;
        }
    }
    free(file_name);
    return ZERO;
}
