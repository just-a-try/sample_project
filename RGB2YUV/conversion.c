#include"rgb2yuv.h"

/* @authhor: Subhash
 * @function: to get the number and validate
 * @param char pointer validation_msg: message to be displayed
 * @return: returns a interger type number after validation
 */
int getNumberAndValidate(char *validation_msg)
{
    int number;

    do
    {
        scanf("%d", &number);

        if(getchar() != '\n')
        {
            while(getchar() != '\n');
            printf("%s\n", validation_msg);
            number = ZERO;
        }

    }while(!number);

    return number;
}

/* @authhor: Subhash
 * @function: to get the yuv raw data file and convert it to a bmp file
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuvToBmpConversion()
{
    FILE*fp;
    char *file_name;
    bool get_again = false;
    int num_of_bytes2pad, raw_yuv_index = 0, raw_rgb_index, pixel_per_row = 1, width, height;
    uint8_t *raw_yuv_data, *raw_rgb_data;

    do{
        printf("Enter the raw file name\n");
        scanf("%ms", &file_name);

        if(!file_name)
        {
            printf("Memory is not allocated successfully\n");
            return false;
        }


        if(!(fp = fopen(file_name, "rb")))
        {
            printf("Given raw file is not present, give a correct file name\n");
            file_name ? free(file_name) : printf("pointer is null cant be freed\n");
            get_again = true;
        }

    }while(get_again);

    printf("file name is %s\n", file_name);
    bmpName.name[0] = 'B';
    bmpName.name[1] = 'M';
    printf("Enter the width of the image\n");
    width = abs(getNumberAndValidate("enter the valid number "));
    printf("Enter the height of the image\n");
    height = abs(getNumberAndValidate("enter the valid number "));
    num_of_bytes2pad = width % NUM_PIXEL_PER_ROW;
    bmpHeader.fileSize = TOTAL_HEADER_SIZE + width * height *
                         BYTES_IN_PIXEL + height * num_of_bytes2pad;
    bmpHeader.creator1 = ZERO;
    bmpHeader.creator2 = ZERO;
    bmpHeader.bmp_offset = TOTAL_HEADER_SIZE;
    dibHeader.header_sz = DIB_HEADER_SIZE;
    dibHeader.width = width;
    dibHeader.height = height;
    dibHeader.nplanes = PLANES;
    dibHeader.bitspp = BITS_IN_PIXEL;
    dibHeader.compress_type = ZERO;
    dibHeader.bmp_bytesz = dibHeader.width * dibHeader.height *
                         BYTES_IN_PIXEL + dibHeader.height * num_of_bytes2pad;
    dibHeader.hres = ZERO;
    dibHeader.vres = ZERO;
    dibHeader.ncolors = ZERO;
    dibHeader.nimpcolors = ZERO;

    raw_rgb_data = calloc(dibHeader.bmp_bytesz,sizeof(char));

    if(!raw_rgb_data)
    {
        printf("memory not allocated\n");
        goto QUIT;
    }

    raw_yuv_data = malloc(dibHeader.width * dibHeader.height * BYTES_IN_PIXEL);

    if(!raw_yuv_data)
    {
        printf("memory not allocated\n");
        raw_rgb_data ? free(raw_rgb_data) : printf("pointer is null cant be freed\n");
        goto QUIT;
    }


    if(!fread(raw_yuv_data, dibHeader.width * dibHeader.height * BYTES_IN_PIXEL, NMEMB, fp))
    {
       printf("fread is not excecuted successfully\n");
       goto QUIT;
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
        file_name ? free(file_name) : printf("pointer is null cant be freed\n");
        raw_yuv_data ? free(raw_yuv_data) : printf("pointer is null cant be freed\n");
        return false;
    }

    fp = NULL;
    raw_rgb_index = dibHeader.bmp_bytesz - ( dibHeader.width * BYTES_IN_PIXEL) - num_of_bytes2pad;

    while(raw_yuv_index < dibHeader.height * dibHeader.width * BYTES_IN_PIXEL - 2)
    {
        raw_rgb_data[raw_rgb_index] = raw_yuv_data[raw_yuv_index] + 2.032 *
                                              (raw_yuv_data[raw_yuv_index + 1] - 128);
        raw_rgb_data[raw_rgb_index + 1] = raw_yuv_data[raw_yuv_index] - 0.395 *
                                              (raw_yuv_data[raw_yuv_index + 1] - 128) -
                                              0.581 * (raw_yuv_data[raw_yuv_index + 2] - 128);
        raw_rgb_data[raw_rgb_index + 2] = raw_yuv_data[raw_yuv_index] + 1.140 *
                                              (raw_yuv_data[raw_yuv_index + 2] - 128);

        pixel_per_row++;

        if( pixel_per_row > dibHeader.width )
        {
            raw_rgb_index -= (( dibHeader.width * 2 - 1) * BYTES_IN_PIXEL + num_of_bytes2pad);
            raw_yuv_index += BYTES_IN_PIXEL;
            pixel_per_row = 0;
        }
        else
        {
            raw_rgb_index += BYTES_IN_PIXEL;
            raw_yuv_index += BYTES_IN_PIXEL;
        }
    }

    if(!fp)
    {
        if(!(fp = fopen("newbmpfile.bmp", "wb")))
        {
            printf("Error in openning the file\n");
            goto QUIT;
        }

        if(fwrite(&bmpName, FORMAT_NAME_SIZE, NMEMB, fp) == EOF)
        {
            printf("fwrite 1 is not excecuted successfully\n");
            goto QUIT;
        }

        if(fwrite(&bmpHeader, BMP_HEADER_SIZE, NMEMB, fp) == EOF)
        {
            printf("fwrite 2 is not excecuted successfully\n");
            goto QUIT;
        }

        if(fwrite(&dibHeader, DIB_HEADER_SIZE, NMEMB, fp) == EOF)
        {
            printf("fwrite 3 is not excecuted successfully\n");
            goto QUIT;
        }

        if(fwrite(raw_rgb_data, dibHeader.bmp_bytesz, NMEMB, fp) == EOF)
        {
            printf("fwrite 4 is not excecuted successfully\n");
            goto QUIT;
        }

        printf("Converted data written successfully in the newbmpfile.bmp\n");

        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
            file_name ? free(file_name) : printf("pointer is null cant be freed\n");
            raw_yuv_data ? free(raw_yuv_data) : printf("pointer is null cant be freed\n");
            return false;
        }
    }
    else
    {
        printf("file pointer is not null\n");
    }

    return true;

    QUIT:

    file_name ? free(file_name) : printf("pointer is null cant be freed\n");
    raw_yuv_data ? free(raw_yuv_data) : printf("pointer is null cant be freed\n");
    raw_rgb_data ? free(raw_rgb_data) : printf("pointer is null cant be freed\n");

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
    }
    return false;

}

/* @authhor: Subhash
 * @function: to get the yuv raw data file and convert it to a bmp file
 * @return: returns a true if function executed successfully or false if it fails
 */
bool bmpToYuvConersion()
{
    FILE *fp;
    int num_of_padding_per_row, raw_rgb_index = 0, new_raw_rgb_index = 0, raw_data_size, pixel_per_row = 0;
    uint8_t *raw_rgb_data, y, u, v;
    char *file_name;
    printf("Enter the BMP file name : ");
    scanf("%ms", &file_name);

    if(!file_name)
    {
        printf("Memory is not allocated successfully\n");
        return false;
    }

    printf("file name is %s\n", file_name);

    if(!(fp = fopen(file_name, "rb")))
    {
        printf("Given BMP file is not present\n");
        file_name ? free(file_name) : printf("pointer is null cant be freed\n");
        return false;
    }

    if(!fread(&bmpName, FORMAT_NAME_SIZE, NMEMB, fp))
    {
       printf("fread 1 is not excecuted successfully\n");
       goto QUIT;
    }

    if(!fread(&bmpHeader, BMP_HEADER_SIZE, NMEMB, fp))
    {
       printf("fread 2 is not excecuted successfully\n");
       goto QUIT;
    }

    if(!fread(&dibHeader, DIB_HEADER_SIZE, NMEMB, fp))
    {
       printf("fread 3 is not excecuted successfully\n");
       goto QUIT;
    }

    raw_rgb_data = malloc(dibHeader.bmp_bytesz);

    if(!raw_rgb_data)
    {
        printf("memory not allocated\n");
        goto QUIT;
    }

    if(!fread(raw_rgb_data, dibHeader.bmp_bytesz, NMEMB, fp))
    {
       printf("fread 4 is not excecuted successfully\n");
       goto QUIT;
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
        file_name ? free(file_name) : printf("pointer is null cant be freed\n");
        raw_rgb_data ? free(raw_rgb_data) : printf("pointer is null cant be freed\n");
        return false;
    }
    printf("name %s\n",bmpName.name);
    printf("file Size %d\n",bmpHeader.fileSize);
    printf("creator1 %d\n",bmpHeader.creator1);
    printf("creator2 %d\n",bmpHeader.creator2);
    printf("bmp_offset %d\n",bmpHeader.bmp_offset);
    printf("header_sz %d\n",dibHeader.header_sz);
    printf("width %d\n",dibHeader.width);
    printf("height %d\n",dibHeader.height);
    printf("nplanes %d\n",dibHeader.nplanes);
    printf("bitspp %d\n",dibHeader.bitspp);
    printf("compress_type %d\n",dibHeader.compress_type);
    printf("bmp_bytesz %d\n",dibHeader.bmp_bytesz);
    printf("hres %d\n",dibHeader.hres);
    printf("vres %d\n",dibHeader.vres);
    printf("ncolors %d\n",dibHeader.ncolors);
    printf("nimpcolors %d\n",dibHeader.nimpcolors);
    fp = NULL;
    raw_data_size = (-dibHeader.height * dibHeader.width * BYTES_IN_PIXEL);
    num_of_padding_per_row = dibHeader.width % NUM_PIXEL_PER_ROW;
    printf("number of padding %d\n",num_of_padding_per_row);

    /* Logic for RGB to YUV conversion :
    * First 3 bytes of the RGB array taken and converted to Y, U, V separately and
    * stored in the same array with same index position where RGB data is taken
    */
    while(raw_rgb_index < dibHeader.bmp_bytesz - (2 + num_of_padding_per_row))
    {
        y = 0.114 * raw_rgb_data[raw_rgb_index] +
                                          0.587 * raw_rgb_data[raw_rgb_index + 1] +
                                          0.299 * raw_rgb_data[raw_rgb_index+2];
        u = 0.492 *(raw_rgb_data[raw_rgb_index] - y) + 128;
        v = 0.877 * (raw_rgb_data[raw_rgb_index+2] - y) + 128;
        raw_rgb_data[new_raw_rgb_index] = y;
        raw_rgb_data[new_raw_rgb_index + 1] = u;
        raw_rgb_data[new_raw_rgb_index + 2] = v;
        pixel_per_row++;

        if(pixel_per_row > dibHeader.width)
        {
            raw_rgb_index = raw_rgb_index + BYTES_IN_PIXEL + num_of_padding_per_row;
            pixel_per_row = 0;
        }
        else
        {
            raw_rgb_index += BYTES_IN_PIXEL;
        }
        new_raw_rgb_index += BYTES_IN_PIXEL;
    }

    if(num_of_padding_per_row)
    {
        raw_rgb_data = realloc(raw_rgb_data, raw_data_size);
        if(!raw_rgb_data)
        {
            printf("Memory is not allocated successfully\n");
            file_name ? free(file_name) : printf("pointer is null cant be freed\n");
            return false;
        }
    }

    if(!fp)
    {
        if(!(fp = fopen("YUV_file.raw", "wb")))
        {
            printf("Error in openning the file\n");
            goto QUIT;
        }

        if(fwrite(raw_rgb_data, raw_data_size, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            goto QUIT;
        }

        printf("Converted data written successfully in the file YUV_file.raw\n");

        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
            file_name ? free(file_name) : printf("pointer is null cant be freed\n");
            raw_rgb_data ? free(raw_rgb_data) : printf("pointer is null cant be freed\n");
            return false;
        }
    }
    else
    {
        printf("file pointer is not null\n");
    }

    file_name ? free(file_name) : printf("pointer is null cant be freed\n");
    raw_rgb_data ? free(raw_rgb_data) : printf("pointer is null cant be freed\n");

    return true;

    QUIT:

    file_name ? free(file_name) : printf("pointer is null cant be freed\n");
    raw_rgb_data ? free(raw_rgb_data) : printf("pointer is null cant be freed\n");

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
    }
    return false;
}

int main()
{
    int choice;
    bool loop_run = true;

    while(loop_run)
    {
        printf("\n         IMAGE CONVERSIONS            \
                \n1-- BMP file to YUV raw file\
                \n2-- YUV raw file to BMP file\
                \n3-- Exit\n");
        choice = getNumberAndValidate("enter the right choice ");

        switch(choice)
        {
            case BMP2YUV:
                if(!bmpToYuvConersion())
                {
                    printf("bmpToYuvConersion function is not executed successfully\n");
                }
                break;
            case YUV2BMP:
                if(!yuvToBmpConversion())
                {
                    printf("YuvtobmpConersion function is not executed successfully\n");
                }
                printf("yuv to bmp conversion\n");
                break;
            case EXIT:
                loop_run = false;
                break;
            default:
                printf("enter the right choice\n");
                break;
        }
    }


    exit(0);

}
