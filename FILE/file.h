#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#define ZERO                      0
#define SEEK_OFFSET               0
#define MINIMUM_WORD_FOUND        0
#define FAILED                   -1
#define NMEMB                     1

enum file_menu
{
    SIZE = 1,
    INSERT,
    FIND,
    REPLACE,
    DISPLAY,
    EXIT
};
enum insert_options
{
    FRONT = 1,
    END,
    POSITION
};
int getNumberAndValidate(char *);
bool displayFile(char*);
bool insert(char*);
int findWord(char*, char*);
char* getContentOfThefile(char*);
bool findAndReplace(char*);
int fileSize(char*);
