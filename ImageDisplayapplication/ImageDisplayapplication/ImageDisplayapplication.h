#pragma once

#include "resource.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include <stdint.h>
#include <math.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "targetver.h"
#include <comdef.h>

#define NMEMB               1
#define FIRST_U_INDEX       1
#define SECOND_U_INDEX      4
#define FIRST_V_INDEX       2
#define SECOND_V_INDEX      5
#define YUV444_INDEX_LIMIT  5
#define FORMAT_NAME_SIZE    2
#define BMP_HEADER_SIZE     12
#define DIB_HEADER_SIZE     40
#define TOTAL_HEADER_SIZE   54
#define BYTES_IN_PIXEL      3
#define BITS_IN_PIXEL       24
#define PLANES_IN_IMAGE     1
#define COLOR_PALLETE_SIZE  1024
#define COLOR_RANGE         256       
#define ZERO                0
#define NUM_PIXEL_PER_ROW   4
#define FILE_NAME          'B','M'
#define SEEK_OFFSET         0

struct bmp_name
{
	unsigned char name[FORMAT_NAME_SIZE];
}bmpName;

struct bmp_header
{
	uint32_t fileSize;
	uint16_t creator1;
	uint16_t creator2;
	uint32_t bmp_offset;
}bmpHeader;

struct dib_header
{
	uint32_t header_sz;
	uint32_t width;
	uint32_t height;
	uint16_t nplanes;
	uint16_t bitspp;
	uint32_t compress_type;
	uint32_t bmp_bytesz;
	uint32_t hres;
	uint32_t vres;
	uint32_t ncolors;
	uint32_t nimpcolors;
}dibHeader;