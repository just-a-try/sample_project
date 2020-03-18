#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define YUV422_FILESIZE_FACTOR            2
#define YUV420_FILESIZE_FACTOR            1.5
#define NMEMB                             1
#define FORMAT_NAME_SIZE                  2
#define BMP_HEADER_SIZE                   12
#define DIB_HEADER_SIZE                   40
#define TOTAL_HEADER_SIZE                 54
#define BYTES_IN_PIXEL                    3
#define BITS_IN_PIXEL                     24
#define PLANES                            1
#define ZERO                              0
#define FAILED                           -1
#define NUM_PIXEL_PER_ROW                 4
#define SEEK_OFFSET                       0

enum zoom_menu
{
    YUV420P_ZOOMING = 1,
    YUV422_ZOOMING,
    EXIT
};

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

bool yuv420Zooming(int);
bool yuv422Zooming(int);
int getNumberAndValidate(char*);
int fileSize(char*);
bool yuvToBmpConversion(char*, int, int);
bool yuv420pToyuv444(char*, int, int);
bool yuv422Toyuv444(char*, int, int);
bool yuv420NewZooming();
bool yuv422NewZooming();
bool yuv422Resizing(char*,int, int, int);
bool yuv420Resizing(char*,int, int, int);
uint8_t* resize422(uint8_t *,int, int, int);
uint8_t* resize420(uint8_t *,int, int, int);
uint8_t* crop422(uint8_t *,int, int, int, int, int);
uint8_t* crop420(uint8_t *, int, int, int, int, int, int);

