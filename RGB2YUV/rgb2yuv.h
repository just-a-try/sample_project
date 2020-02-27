#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include <stdint.h>
#define NMEMB               1
#define FORMAT_NAME_SIZE    2
#define BMP_HEADER_SIZE     12
#define DIB_HEADER_SIZE     40
#define BYTES_IN_PIXEL      3

struct bmp_name
{
    unsigned char name[2];
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
