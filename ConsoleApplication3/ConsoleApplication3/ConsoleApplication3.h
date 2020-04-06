#include <iostream>
#include<Windows.h>
#include <process.h>
#define NMEB 1
#pragma warning(disable : 4996)

#pragma pack(1)
typedef struct time
{
	WORD hour;
	WORD minute;
}TIME;