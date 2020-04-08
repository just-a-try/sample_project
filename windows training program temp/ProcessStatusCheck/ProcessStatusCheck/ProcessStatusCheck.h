#include <Windows.h>
#include <tlhelp32.h>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <psapi.h>
#pragma once
bool IsProcessRunning(const wchar_t *);

#pragma once