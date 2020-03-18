#include "crop_and_zoom.h"

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
 * @function: to get the size of the file
 * @param char pointer file_name: string of the file name
 * @return: returns the interger type file size or FAILED macro if function fails
 */
int fileSize(char *file_name)
{
    FILE *fp;
    int file_size;

    if(!(fp = fopen(file_name, "rb")))
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
 * @function: to crop the data from given raw data pointer array
 * @param unsigned 8 bit integer pointer array raw_yuv420_data: array with the raw YUV420 data
 * @param integer x_coordinate: row position of the pixel from where crop to be done
 * @param integer y_coordinate: column position of the pixel from where crop to be done
 * @param integer width: width of the given raw image
 * @param integer crop_width: width of the image to be cropped
 * @param integer crop_height: height of the image to be cropped
 * @return: unsigned 8 bit integer pointer array which is cropped
 */
uint8_t * crop420(uint8_t * raw_yuv420_data, int x_coordinate, int y_coordinate, int width, int height,
            int crop_width, int crop_height)
{
    int cropped_size, Y420c_index = 0, row, original_usize, col, U420c_index,
        Y420_index, cropped_usize, u_index, crop_pixel_index, u1, u2, u3, u4;
    uint8_t * cropped_yuv420_data = NULL;

    if(!raw_yuv420_data)
    {
        printf("passed raw data array is NULL\n");
        return NULL;
    }

    crop_pixel_index = (x_coordinate - 1) * width + (y_coordinate - 1);
    u_index = width * height;
    U420c_index = crop_width * crop_height;


    cropped_size = crop_width * crop_height * YUV420_FILESIZE_FACTOR;
    cropped_usize = (crop_width / 2) * (crop_height / 2);
    original_usize = (width / 2) * (height / 2);

    cropped_yuv420_data = (uint8_t *)malloc(cropped_size * sizeof(uint8_t));

    if(!cropped_yuv420_data)
    {
        printf("memory not allocated\n");
        return NULL;
    }

    for(row = crop_pixel_index;row <= crop_pixel_index + (width * (crop_height - 2));row += width * 2)
    {
        Y420_index = row;

        for(col = 0;col < crop_width / 2;col++)
        {
            cropped_yuv420_data[Y420c_index] = raw_yuv420_data[Y420_index];
            cropped_yuv420_data[Y420c_index + 1] = raw_yuv420_data[Y420_index + 1];
            cropped_yuv420_data[Y420c_index + crop_width] = raw_yuv420_data[Y420_index + width];
            cropped_yuv420_data[Y420c_index + crop_width + 1] = raw_yuv420_data[Y420_index + width + 1];

            u1 = u_index + (Y420_index % width) / 2 + (width / 2) * ((Y420_index / (2 * width)) % width);
            u2 = u_index + ((Y420_index + 1) % width) / 2 + (width / 2) * (((Y420_index + 1) / (2 * width)) % width);
            u3 = u_index + ((Y420_index + width) % width) / 2 + (width / 2) * (((Y420_index + width) / (2 * width)) % width);
            u4 = u_index + ((Y420_index + width + 1) % width) / 2 + (width / 2) * (((Y420_index + width + 1) / (2 * width)) % width);

            cropped_yuv420_data[U420c_index] = (raw_yuv420_data[u1] + raw_yuv420_data[u2] +
                                                raw_yuv420_data[u3] + raw_yuv420_data[u4]) / 4;
            cropped_yuv420_data[U420c_index + cropped_usize] = (raw_yuv420_data[u1 + original_usize] +
                                                               raw_yuv420_data[u2 + original_usize] +
                                                               raw_yuv420_data[u3 + original_usize] +
                                                               raw_yuv420_data[u4 + original_usize]) / 4;
            Y420_index += 2;
            U420c_index++;
            Y420c_index += 2;
        }

        Y420c_index += crop_width;
    }

    return cropped_yuv420_data;
}


/* @authhor: Subhash
 * @function: to crop the data from given raw data pointer array
 * @param unsigned 8 bit integer pointer raw_yuv422_data: array with the raw YUV422 data
 * @param integer crop_pixel_index: index from where the crop to be done
 * @param integer width: width of the given raw image
 * @param integer crop_width: width of the image to be cropped
 * @param integer crop_height: height of the image to be cropped
 * @param integer y_coordinate: y_coordinate of the pixel from where the image to be cropped
 * @return: unsigned 8 bit integer pointer array which is cropped
 */
uint8_t * crop422(uint8_t * raw_yuv422_data, int crop_pixel_index, int width,
                  int crop_width, int crop_height, int y_coordinate)
{
    int cropped_size, Y422c_index = 1, row, col, U422_index, U422c_index = 0, Y422_index = 1;
    uint8_t * cropped_yuv422_data = NULL;

    if(!raw_yuv422_data)
    {
        printf("passed raw data array is NULL\n");
        return NULL;
    }

    cropped_size = crop_width * crop_height * YUV422_FILESIZE_FACTOR;

    cropped_yuv422_data = (uint8_t *)malloc(cropped_size * sizeof(uint8_t));

    if(!cropped_yuv422_data)
    {
        printf("memory not allocated\n");
        return NULL;
    }

    for(row = crop_pixel_index;row <= crop_pixel_index + (width * 2 * (crop_height - 1));row += width * 2)
    {
        col = 0;
        for(Y422_index = row;col < crop_width;col++)
        {
            cropped_yuv422_data[Y422c_index] = raw_yuv422_data[Y422_index];
            Y422_index += 2;
            Y422c_index += 2;
        }
    }

    if(y_coordinate % 2)
    {
        row = crop_pixel_index - 1;
    }
    else
    {
        row = crop_pixel_index - 3;
    }

    for(;row <= (crop_pixel_index - 1) + (width * 2 * (crop_height - 1));row += width * 2)
    {
        U422_index = row;
        for(col = 0;col < crop_width / 2;col++)
        {
            if(y_coordinate % 2)
            {
                cropped_yuv422_data[U422c_index] = raw_yuv422_data[U422_index];
                cropped_yuv422_data[U422c_index + 2] = raw_yuv422_data[U422_index + 2];
            }
            else
            {
                cropped_yuv422_data[U422c_index] = (raw_yuv422_data[U422_index] +
                                                    raw_yuv422_data[U422_index + 4]) / 2;
                cropped_yuv422_data[U422c_index + 2] = (raw_yuv422_data[U422_index + 2] +
                                                        raw_yuv422_data[U422_index + 6]) / 2;
            }

            U422_index += 4;
            U422c_index += 4;
        }
    }

    return cropped_yuv422_data;
}

/* @authhor: Subhash
 * @function: to zoom the data from given raw data pointer array
 * @param unsigned 8 bit integer pointer raw_yuv420_data: array with the raw YUV420 data
 * @param integer width: width of the given raw image
 * @param integer height: height of the given raw imaged
 * @param integer zoom_factor: factor to which the pixels to be resized
 * @return: unsigned 8 bit integer pointer array which is resized
 */
uint8_t * resize420(uint8_t *raw_yuv420_data, int width, int height, int zoom_factor)
{
    int YUV420_filesize, zoom_file_size, Y420_index, pixels,
        row, col, flag1, flag2, Y420z_index = 0;
    uint8_t *zoomed_yuv420_data = NULL;

    if(!raw_yuv420_data)
    {
        printf("passed raw data array is NULL\n");
        return NULL;
    }

    pixels = width * height;
    YUV420_filesize = pixels * YUV420_FILESIZE_FACTOR;
    zoom_file_size = (width * zoom_factor) * (height * zoom_factor) * YUV420_FILESIZE_FACTOR;

    zoomed_yuv420_data = (uint8_t *)malloc(zoom_file_size * sizeof(uint8_t));

    if(!zoomed_yuv420_data)
    {
        printf("memory not allocated\n");
        return NULL;
    }

    for(row = 0;row <= pixels - width;row += width)
    {
        for(flag1 = 0;flag1 < zoom_factor;flag1++)
        {
            for(Y420_index = row,col = 0;col < width;col++,Y420_index++)
            {
                for(flag2 = 0;flag2 < zoom_factor;flag2++)
                {
                    zoomed_yuv420_data[Y420z_index++] = raw_yuv420_data[Y420_index];
                }
            }
        }
    }

    for(row = pixels;row <= YUV420_filesize - width / 2;row += width / 2)
    {
        for(flag1 = 0;flag1 < zoom_factor;flag1++)
        {
            for(Y420_index = row,col = 0;col < width / 2;col++,Y420_index++)
            {
                for(flag2 = 0;flag2 < zoom_factor;flag2++)
                {
                    zoomed_yuv420_data[Y420z_index++] = raw_yuv420_data[Y420_index];
                }
            }
        }
    }

    return zoomed_yuv420_data;
}

/* @authhor: Subhash
 * @function: to zoom the data from given raw data pointer array
 * @param unsigned 8 bit integer pointer raw_yuv422_data: array with the raw YUV422 data
 * @param integer width: width of the given raw image
 * @param integer height: height of the given raw imaged
 * @param integer zoom_factor: factor to which the pixels to be resized
 * @return: unsigned 8 bit integer pointer array which is resized
 */
uint8_t* resize422(uint8_t *raw_yuv422_data, int width, int height, int zoom_factor)
{
    int YUV422_filesize, zoom_file_size, Y422_index,
        row, col, flag1, flag2, Y422z_index;
    uint8_t *zoomed_yuv422_data = NULL;

    if(!raw_yuv422_data)
    {
        printf("passed raw data array is NULL\n");
        return NULL;
    }

    YUV422_filesize = width * height * YUV422_FILESIZE_FACTOR;
    zoom_file_size = (width * zoom_factor) * (height * zoom_factor) * YUV422_FILESIZE_FACTOR;

    zoomed_yuv422_data = (uint8_t *)malloc(zoom_file_size * sizeof(uint8_t));

    if(!zoomed_yuv422_data)
    {
        printf("memory not allocated\n");
        return NULL;
    }

    Y422z_index = 1;

    for(row = 1;row < YUV422_filesize - width * 2 + 1;row += width * 2)
    {
        for(flag1 = 0;flag1 < zoom_factor;flag1++)
        {
            for(Y422_index = row,col = 0;col < width;col++,Y422_index += 2)
            {
                for(flag2 = 0;flag2 < zoom_factor;flag2++)
                {
                    zoomed_yuv422_data[Y422z_index] = raw_yuv422_data[Y422_index];
                    Y422z_index += 2;
                }
            }
        }
    }

    Y422z_index = 0;

    for(row = 0;row <= YUV422_filesize - width * 2;row += width * 2)
    {
        for(flag1 = 0;flag1 < zoom_factor;flag1++)
        {
            for(Y422_index = row,col = 0;col < width / 2;col++,Y422_index += 4)
            {
                for(flag2 = 0;flag2 < zoom_factor;flag2++)
                {
                    zoomed_yuv422_data[Y422z_index] = raw_yuv422_data[Y422_index];
                    zoomed_yuv422_data[Y422z_index + 2] = raw_yuv422_data[Y422_index + 2];
                    Y422z_index += 4;
                }
            }
        }
    }

    return zoomed_yuv422_data;
}

/* @authhor: Subhash
 * @function: to resize the yuv420 file with given zoomfactor
 * @param character pointer file_name: name fo the file to be resized
 * @param integer width: width of the given raw image file
 * @param integer height: height of the given raw image file
 * @param integer zoom_factor: zoom factor to which the image to be resized
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv420Resizing(char *file_name, int width, int height, int zoom_factor)
{
    bool function_status = true;
    int YUV420_filesize, zoom_file_size;
    uint8_t *raw_yuv420_data = NULL, *zoomed_yuv420_data = NULL;
    FILE *fp = NULL;

    if(!(fp = fopen(file_name, "rb")))
    {
        printf("Given file is not present\n");
        function_status = false;
        goto QUIT;
    }

    YUV420_filesize = width * height * YUV420_FILESIZE_FACTOR;
    zoom_file_size = (width * zoom_factor) * (height * zoom_factor)* YUV420_FILESIZE_FACTOR;

    raw_yuv420_data = (uint8_t *)malloc(YUV420_filesize * sizeof(uint8_t));

    if(!raw_yuv420_data)
    {
        printf("memory not allocated\n");
        function_status = false;
        goto QUIT;
    }

    if(!fread(raw_yuv420_data, YUV420_filesize, NMEMB, fp))
    {
       printf("fread is not excecuted successfully\n");
       function_status = false;
       goto QUIT;
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
        function_status = false;
        goto QUIT;
    }

    fp = NULL;

    zoomed_yuv420_data = resize420(raw_yuv420_data, width, height, zoom_factor);

    if(!zoomed_yuv420_data)
    {
        printf("zoomed array pointer is NULL\n");
        function_status = false;
        goto QUIT;
    }

    if(!fp)
    {
        if(!(fp = fopen("YUV420_zoomed.raw", "wb")))
        {
            printf("Error in openning the file\n");
            function_status = false;
            goto QUIT;
        }

        if(fwrite(zoomed_yuv420_data, zoom_file_size, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            function_status = false;
            goto QUIT;
        }

        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
            function_status = false;
            goto QUIT;
        }

        fp = NULL;

    }
    else
    {
        printf("file pointer is not null\n");
    }

    width *= zoom_factor;
    height *= zoom_factor;

    if(!yuv420pToyuv444("YUV420_zoomed.raw", width, height))
    {
        printf(" yuv420pToyuv444 function is not executed successfully\n");
        function_status = false;
    }

    if(!yuvToBmpConversion("YUV_zoomed_file.raw", width, height))
    {
        printf(" yuvToBmpConversion function is not executed successfully\n");
        function_status = false;
    }

    QUIT:

    if(zoomed_yuv420_data)
        free(zoomed_yuv420_data);

    if(raw_yuv420_data)
        free(raw_yuv420_data);

    if(fp)
    {
        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
        }
    }

    return function_status;
}

/* @authhor: Subhash
 * @function: to resize the yuv422 file with given zoomfactor
 * @param character pointer file_name: name fo the file to be resized
 * @param integer width: width of the given raw image file
 * @param integer height: height of the given raw image file
 * @param integer zoom_factor: zoom factor to which the image to be resized
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv422Resizing(char *file_name, int width, int height, int zoom_factor)
{
    bool function_status = true;
    int YUV422_filesize, zoom_file_size;
    uint8_t *raw_yuv422_data = NULL, *zoomed_yuv422_data = NULL;
    FILE *fp = NULL;

    if(!(fp = fopen(file_name, "rb")))
    {
        printf("Given file is not present\n");
        function_status = false;
        goto QUIT;
    }

    YUV422_filesize = width * height * YUV422_FILESIZE_FACTOR;
    zoom_file_size = (width * zoom_factor) * (height * zoom_factor)* YUV422_FILESIZE_FACTOR;

    raw_yuv422_data = (uint8_t *)malloc(YUV422_filesize * sizeof(uint8_t));

    if(!raw_yuv422_data)
    {
        printf("memory not allocated\n");
        function_status = false;
        goto QUIT;
    }

    if(!fread(raw_yuv422_data, YUV422_filesize, NMEMB, fp))
    {
       printf("fread is not excecuted successfully\n");
       function_status = false;
       goto QUIT;
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
        function_status = false;
        goto QUIT;
    }

    fp = NULL;

    zoomed_yuv422_data = resize422(raw_yuv422_data, width, height, zoom_factor);

    if(!zoomed_yuv422_data)
    {
        printf("zoomed array pointer is NULL\n");
        function_status = false;
        goto QUIT;
    }

    if(!fp)
    {
        if(!(fp = fopen("YUV422_zoomed.raw", "wb")))
        {
            printf("Error in openning the file\n");
            function_status = false;
            goto QUIT;
        }

        if(fwrite(zoomed_yuv422_data, zoom_file_size, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            function_status = false;
            goto QUIT;
        }

        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
            function_status = false;
            goto QUIT;
        }
        fp = NULL;

    }
    else
    {
        printf("file pointer is not null\n");
    }

    width *= zoom_factor;
    height *= zoom_factor;

    if(!yuv422Toyuv444("YUV422_zoomed.raw", width, height))
    {
        printf(" yuv422Toyuv444 function is not executed successfully\n");
        function_status = false;
    }

    if(!yuvToBmpConversion("YUV_zoomed_file.raw", width, height))
    {
        printf(" yuvToBmpConversion function is not executed successfully\n");
        function_status = false;
    }

    QUIT:

    if(zoomed_yuv422_data)
        free(zoomed_yuv422_data);

    if(raw_yuv422_data)
        free(raw_yuv422_data);

    if(fp)
    {
        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
        }
    }

    return function_status;
}

/* @authhor: Subhash
 * @function: to zoom the given YUV420 raw image
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv420NewZooming()
{
    bool function_status = true;

    int width, height, YUV420_filesize, file_size, crop_width, zoom_factor,
        crop_height, x_coordinate, y_coordinate, crop_pixel_index, cropped_size;

    uint8_t *raw_yuv420_data = NULL, *cropped_yuv420_data = NULL;
    FILE *fp = NULL;
    char *file_name = NULL;

    do{

        if(file_name)
        {
            printf("Given raw file is not present, give a correct file name\n");

            if(file_name)
                free(file_name);
        }

        printf("Enter the YUV420 raw file name\n");
        scanf("%ms", &file_name);

        if(!file_name)
        {
            printf("Memory is not allocated successfully\n");
            function_status = false;
            goto QUIT;
        }

    }while(!(fp = fopen(file_name, "rb")));

    if((file_size = fileSize(file_name)) == FAILED)
    {
        printf("Error in fileSize function\n");
        function_status = false;
        goto QUIT;
    }

    while(1)
    {
        printf("Enter the width of the original image\n");
        width = abs(getNumberAndValidate("enter the valid number "));

        printf("Enter the height of the original image\n");
        height = abs(getNumberAndValidate("enter the valid number "));

        YUV420_filesize = width * height * YUV420_FILESIZE_FACTOR;

        if(file_size != YUV420_filesize)
        {
            goto get_again;
        }

        printf("Enter the x coordinate of the pixel in the image\n");
        x_coordinate = abs(getNumberAndValidate("enter the valid number "));

        printf("Enter the y coordinate of the pixel in the image\n");
        y_coordinate = abs(getNumberAndValidate("enter the valid number "));

        printf("Enter the width to which image to be cropped\n");
        crop_width = abs(getNumberAndValidate("enter the valid number "));

        printf("Enter the height to which image to be cropped\n");
        crop_height = abs(getNumberAndValidate("enter the valid number "));

        do
        {
            printf("Enter the zoom factor between 2x to 10x\n");
            zoom_factor = getNumberAndValidate("enter the right number \n");

        }while(zoom_factor < 1 || zoom_factor > 11);

        if(crop_width > (width - (y_coordinate - 1)))
        {
            crop_width = width - (y_coordinate - 1);
            printf("Given crop width exceeded the image edge so reduced to %d\n",crop_width);
        }

        if(crop_height > (height - (x_coordinate - 1)))
        {
            crop_height = height - (x_coordinate - 1);
            printf("Given crop height exceeded the image edge so reduced to %d\n",crop_height);
        }

        break;

        get_again:
            printf("Given size does not match with original file size please give correct size\n");
    }

    raw_yuv420_data = (uint8_t *)malloc(YUV420_filesize * sizeof(uint8_t));

    if(!raw_yuv420_data)
    {
        printf("memory not allocated\n");
        function_status = false;
        goto QUIT;
    }

    if(!fread(raw_yuv420_data, YUV420_filesize, NMEMB, fp))
    {
       printf("fread is not excecuted successfully\n");
       function_status = false;
       goto QUIT;
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
        function_status = false;
        goto QUIT;
    }

    fp = NULL;

    cropped_size = crop_width * crop_height * YUV420_FILESIZE_FACTOR;

    cropped_yuv420_data = crop420(raw_yuv420_data, x_coordinate, y_coordinate, width, height,
                                  crop_width, crop_height);
    if(!cropped_yuv420_data)
    {
        printf("cropped array pointer is NULL\n");
        function_status = false;
        goto QUIT;
    }

    if(!fp)
    {
      if(!(fp = fopen("YUV420_cropped.raw", "wb")))
      {
          printf("Error in openning the file\n");
          function_status = false;
          goto QUIT;
      }

      if(fwrite(cropped_yuv420_data, cropped_size, NMEMB, fp) == EOF)
      {
          printf("fwrite is not excecuted successfully\n");
          function_status = false;
          goto QUIT;
      }

      if(fclose(fp) == EOF)
      {
          printf("The file is not closed successfully\n");
          function_status = false;
          goto QUIT;
      }

      fp = NULL;
    }
    else
    {
        printf("file pointer is not null\n");
    }

    if(!yuv420Resizing("YUV420_cropped.raw", crop_width, crop_height, zoom_factor))
    {
        printf("yuv420Resizing function is not executed successfully\n");
        function_status = false;
    }

    QUIT:

    if(file_name)
      free(file_name);

    if(cropped_yuv420_data)
      free(cropped_yuv420_data);

    if(raw_yuv420_data)
      free(raw_yuv420_data);

    if(fp)
    {
        if(fclose(fp) == EOF)
        {
          printf("The file is not closed successfully\n");
        }
    }

    return function_status;
}

/* @authhor: Subhash
 * @function: to zoom the given YUV422 raw image
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv422NewZooming()
{
    bool function_status = true;

    int width, height, YUV422_filesize, file_size,crop_width, zoom_factor,
        crop_height, x_coordinate, y_coordinate, crop_pixel_index, cropped_size;

    uint8_t *raw_yuv422_data = NULL, *cropped_yuv422_data = NULL;
    FILE *fp = NULL;
    char *file_name = NULL;

    do{

        if(file_name)
        {
            printf("Given raw file is not present, give a correct file name\n");

            if(file_name)
                free(file_name);
        }

        printf("Enter the YUV422 raw file name\n");
        scanf("%ms", &file_name);

        if(!file_name)
        {
            printf("Memory is not allocated successfully\n");
            function_status = false;
            goto QUIT;
        }

    }while(!(fp = fopen(file_name, "rb")));

    if((file_size = fileSize(file_name)) == FAILED)
    {
        printf("Error in fileSize function\n");
        function_status = false;
        goto QUIT;
    }

    while(1)
    {
        printf("Enter the width of the original image\n");
        width = abs(getNumberAndValidate("enter the valid number "));

        printf("Enter the height of the original image\n");
        height = abs(getNumberAndValidate("enter the valid number "));

        YUV422_filesize = width * height * YUV422_FILESIZE_FACTOR;

        if(file_size != YUV422_filesize)
        {
            goto get_again;
        }

        printf("Enter the x coordinate of the pixel in the image\n");
        x_coordinate = abs(getNumberAndValidate("enter the valid number "));

        printf("Enter the y coordinate of the pixel in the image\n");
        y_coordinate = abs(getNumberAndValidate("enter the valid number "));

        printf("Enter the width to which image to be cropped\n");
        crop_width = abs(getNumberAndValidate("enter the valid number "));

        printf("Enter the height to which image to be cropped\n");
        crop_height = abs(getNumberAndValidate("enter the valid number "));

        do
        {
            printf("Enter the zoom factor between 2x to 10x\n");
            zoom_factor = getNumberAndValidate("enter the right number \n");

        }while(zoom_factor < 1 || zoom_factor > 11);

        if(crop_width > (width - (y_coordinate - 1)))
        {
            crop_width = width - (y_coordinate - 1);
            printf("Given crop width exceeded the image edge so reduced to %d\n",crop_width);
        }

        if(crop_height > (height - (x_coordinate - 1)))
        {
            crop_height = height - (x_coordinate - 1);
            printf("Given crop height exceeded the image edge so reduced to %d\n",crop_height);
        }

        break;

        get_again:
            printf("Given size does not match with original file size please give correct size\n");
    }

    raw_yuv422_data = (uint8_t *)malloc(YUV422_filesize * sizeof(uint8_t));

    if(!raw_yuv422_data)
    {
        printf("memory not allocated\n");
        function_status = false;
        goto QUIT;
    }

    if(!fread(raw_yuv422_data, YUV422_filesize, NMEMB, fp))
    {
       printf("fread is not excecuted successfully\n");
       function_status = false;
       goto QUIT;
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
        function_status = false;
        goto QUIT;
    }

    fp = NULL;

    crop_pixel_index = 1 +  (x_coordinate - 1) * (2 * width) + (y_coordinate - 1) * 2;
    cropped_size = crop_width * crop_height * YUV422_FILESIZE_FACTOR;

    cropped_yuv422_data = crop422(raw_yuv422_data, crop_pixel_index, width,
                                  crop_width, crop_height, y_coordinate);

    if(!cropped_yuv422_data)
    {
        printf("cropped array pointer is NULL\n");
        function_status = false;
        goto QUIT;
    }

    if(!fp)
    {
      if(!(fp = fopen("YUV422_cropped.raw", "wb")))
      {
          printf("Error in openning the file\n");
          function_status = false;
          goto QUIT;
      }

      if(fwrite(cropped_yuv422_data, cropped_size, NMEMB, fp) == EOF)
      {
          printf("fwrite is not excecuted successfully\n");
          function_status = false;
          goto QUIT;
      }

      if(fclose(fp) == EOF)
      {
          printf("The file is not closed successfully\n");
          function_status = false;
          goto QUIT;
      }

      fp = NULL;
    }
    else
    {
        printf("file pointer is not null\n");
    }

    if(!yuv422Resizing("YUV422_cropped.raw", crop_width, crop_height, zoom_factor))
    {
        printf(" yuv422Resizing function is not executed successfully\n");
        function_status = false;
    }

    QUIT:

    if(file_name)
      free(file_name);

    if(cropped_yuv422_data)
      free(cropped_yuv422_data);

    if(raw_yuv422_data)
      free(raw_yuv422_data);

    if(fp)
    {
        if(fclose(fp) == EOF)
        {
          printf("The file is not closed successfully\n");
        }
    }

    return function_status;
}

/* @authhor: Subhash
 * @function: to get the yuv420P raw data file and convert it to yuv444 format
 * @param character pointer file_name : name of the raw file to be Converted
 * @param interger width: width of the given raw file
 * @param interger height: height of the given raw file
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv420pToyuv444(char *file_name, int width, int height)
{
    FILE *fp = NULL;
    bool function_status = true;
    int flag = 0, pixels, YUV420p_filesize, YUV444_filesize,
        Y420p_index = 0, Y444_index = 0, U444_index = 1, V444_index = 2;
    uint8_t *raw_yuv420p_data, *raw_yuv444_data;

    if(!(fp = fopen(file_name, "rb")))
    {
        printf("Given file is not present\n");
        function_status = false;
        goto QUIT;
    }

    pixels = width * height;
    YUV444_filesize = pixels * BYTES_IN_PIXEL;
    YUV420p_filesize = pixels + 2 * ((width / 2) * (height / 2));

    raw_yuv420p_data = (uint8_t *)malloc(YUV420p_filesize * sizeof(uint8_t));

    if(!raw_yuv420p_data)
    {
        printf("memory not allocated\n");
        function_status = false;
        goto QUIT;
    }

    raw_yuv444_data = (uint8_t *)malloc(YUV444_filesize * sizeof(uint8_t));

    if(!raw_yuv444_data)
    {
        printf("memory not allocated\n");
        function_status = false;
        goto QUIT;
    }

    if(!fread(raw_yuv420p_data, YUV420p_filesize, NMEMB, fp))
    {
       printf("fread is not excecuted successfully\n");
       function_status = false;
       goto QUIT;
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
        function_status = false;
        goto QUIT;
    }

    fp = NULL;

    while(Y420p_index < YUV420p_filesize)
    {
          if(Y420p_index < pixels)
          {
              raw_yuv444_data[Y444_index] = raw_yuv420p_data[Y420p_index];
              Y444_index += 3;
              Y420p_index++;
          }
          else if(Y420p_index < pixels + ((width / 2) * (height / 2)))
          {
              raw_yuv444_data[U444_index] = raw_yuv444_data[U444_index + 3] =
              raw_yuv444_data[U444_index + BYTES_IN_PIXEL * width] =
              raw_yuv444_data[U444_index + BYTES_IN_PIXEL * width + 3] = raw_yuv420p_data[Y420p_index];
              flag += 2;

              if(flag == width)
              {
                  U444_index += (width + 2) * BYTES_IN_PIXEL;
                  flag = 0;
              }
              else
              {
                  U444_index += 6;
              }

              Y420p_index++;
          }
          else
          {
              raw_yuv444_data[V444_index] = raw_yuv444_data[V444_index + 3] =
              raw_yuv444_data[V444_index + BYTES_IN_PIXEL * width] =
              raw_yuv444_data[V444_index + BYTES_IN_PIXEL * width + 3] = raw_yuv420p_data[Y420p_index];
              flag += 2;

              if(flag == width)
              {
                  V444_index += (width + 2) * BYTES_IN_PIXEL;
                  flag = 0;
              }
              else
              {
                  V444_index += 6;
              }

              Y420p_index++;

          }

    }

    if(!fp)
    {
        if(!(fp = fopen("YUV_zoomed_file.raw", "wb")))
        {
            printf("Error in openning the file\n");
            function_status = false;
            goto QUIT;
        }

        if(fwrite(raw_yuv444_data, YUV444_filesize, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            function_status = false;
            goto QUIT;
        }

        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
            function_status = false;
            goto QUIT;
        }
        fp = NULL;

    }
    else
    {
        printf("file pointer is not null\n");
    }

    QUIT:

    if(raw_yuv444_data)
        free(raw_yuv444_data);

    if(raw_yuv420p_data)
        free(raw_yuv420p_data);

    if(fp)
    {
        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
        }
    }

    return function_status;
}

/* @authhor: Subhash
 * @function: to get the yuv422 raw data file and convert it to yuv444 format
 * @param character pointer file_name: name of the raw file to be converted
 * @param integer width: width of the given raw file
 * @param ineteger height: height of the given raw file
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv422Toyuv444(char *file_name, int width, int height)
{
    FILE *fp = NULL;
    bool function_status = true;
    int YUV444_filesize, YUV422_filesize, Y444_index = 0, Y422_index = 0;
    uint8_t *raw_yuv444_data, *raw_yuv422_data;

    if(!(fp = fopen(file_name, "rb")))
    {
        printf("Given file is not present\n");
        function_status = false;
        goto QUIT;
    }

    YUV444_filesize = width * height * BYTES_IN_PIXEL;
    YUV422_filesize = YUV444_filesize - width * height;

    raw_yuv422_data = (uint8_t *)malloc(YUV422_filesize * sizeof(uint8_t));

    if(!raw_yuv422_data)
    {
        printf("memory not allocated\n");
        function_status = false;
        goto QUIT;
    }

    raw_yuv444_data = (uint8_t *)malloc(YUV444_filesize * sizeof(uint8_t));

    if(!raw_yuv444_data)
    {
        printf("memory not allocated\n");
        function_status = false;
        goto QUIT;
    }

    if(!fread(raw_yuv422_data, YUV422_filesize, NMEMB, fp))
    {
        printf("fread is not excecuted successfully\n");
        function_status = false;
        goto QUIT;
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
        function_status = false;
        goto QUIT;
    }

    fp = NULL;

    while(Y422_index < YUV422_filesize - 3)
    {
        raw_yuv444_data[Y444_index++] = raw_yuv422_data[Y422_index + 1];
        raw_yuv444_data[Y444_index++] = raw_yuv422_data[Y422_index];
        raw_yuv444_data[Y444_index++] = raw_yuv422_data[Y422_index + 2];
        raw_yuv444_data[Y444_index++] = raw_yuv422_data[Y422_index + 3];
        raw_yuv444_data[Y444_index++] = raw_yuv422_data[Y422_index];
        raw_yuv444_data[Y444_index++] = raw_yuv422_data[Y422_index + 2];
        Y422_index += 4;
    }

    if(!fp)
    {
        if(!(fp = fopen("YUV_zoomed_file.raw", "wb")))
        {
            printf("Error in openning the file\n");
            function_status = false;
            goto QUIT;
        }

        if(fwrite(raw_yuv444_data, YUV444_filesize, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            function_status = false;
            goto QUIT;
        }

        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
            function_status = false;
            goto QUIT;
        }
        fp = NULL;
    }
    else
    {
        printf("file pointer is not null\n");
    }

    QUIT:

    if(raw_yuv444_data)
        free(raw_yuv444_data);

    if(raw_yuv422_data)
        free(raw_yuv422_data);

    if(fp)
    {
        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
        }
    }

    return function_status;
}

/* @authhor: Subhash
 * @function: to get the yuv raw data file and convert it to a bmp file
 * @param character pointer file_name: name fo the file to be Converted
 * @param integer width: width of the given raw file
 * @param ineteger height: height of the given raw file
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuvToBmpConversion(char *file_name, int width, int height)
{
    FILE *fp = NULL;
    bool function_status = true;
    int num_of_bytes2pad, YUV444_filesize, raw_yuv_index = 0, raw_rgb_index,
        pixel_per_row = 1;
    uint8_t *raw_yuv_data, *raw_rgb_data;

    if(!(fp = fopen(file_name, "rb")))
    {
        printf("Given file is not present\n");
        function_status = false;
        goto QUIT;
    }

    YUV444_filesize = width * height * BYTES_IN_PIXEL;
    bmpName.name[0] = 'B';
    bmpName.name[1] = 'M';
    num_of_bytes2pad = width % NUM_PIXEL_PER_ROW;
    bmpHeader.fileSize = TOTAL_HEADER_SIZE + YUV444_filesize + height * num_of_bytes2pad;
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

    raw_rgb_data = (uint8_t *)calloc(dibHeader.bmp_bytesz,sizeof(uint8_t));

    if(!raw_rgb_data)
    {
        printf("memory not allocated\n");
        function_status = false;
        goto QUIT;
    }

    raw_yuv_data = (uint8_t *)malloc(dibHeader.width * dibHeader.height * BYTES_IN_PIXEL * sizeof(uint8_t));

    if(!raw_yuv_data)
    {
        printf("memory not allocated\n");
        function_status = false;
        goto QUIT;
    }

    if(!fread(raw_yuv_data, dibHeader.width * dibHeader.height * BYTES_IN_PIXEL, NMEMB, fp))
    {
       printf("fread is bmp not excecuted successfully\n");
       function_status = false;
       goto QUIT;
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
        function_status = false;
        goto QUIT;
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
        if(!(fp = fopen("NewZoomed_file.bmp", "wb")))
        {
            printf("Error in openning the file\n");
            function_status = false;
            goto QUIT;
        }

        if(fwrite(&bmpName, FORMAT_NAME_SIZE, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            function_status = false;
            goto QUIT;
        }

        if(fwrite(&bmpHeader, BMP_HEADER_SIZE, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            function_status = false;
            goto QUIT;
        }

        if(fwrite(&dibHeader, DIB_HEADER_SIZE, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            function_status = false;
            goto QUIT;
        }

        if(fwrite(raw_rgb_data, dibHeader.bmp_bytesz, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            function_status = false;
            goto QUIT;
        }

        printf("Converted data written successfully in the NewZoomed_file.bmp\n");

        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
            function_status = false;
            goto QUIT;
        }
        fp = NULL;
    }
    else
    {
        printf("file pointer is not null\n");
    }
    QUIT:

    if(raw_yuv_data)
        free(raw_yuv_data);

    if(raw_rgb_data)
        free(raw_rgb_data);

    if(fp)
    {
        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
        }
    }

    return function_status;
}

int main()
{
    int choice;
    bool loop_run = true;

    while(loop_run)
    {
        printf("\nEnter your choice\
                \n1-- YUV420P_ZOOMING\
                \n2-- YUV422_ZOOMING\
                \n3-- Exit\n");
        choice = getNumberAndValidate("enter the right choice ");

        switch(choice)
        {
            case YUV420P_ZOOMING:
                if(!yuv420NewZooming())
                {
                    printf(" yuv420NewZooming function is not executed successfully\n");
                }
                break;
            case YUV422_ZOOMING:
                if(!yuv422NewZooming())
                {
                    printf(" yuv422NewZooming function is not executed successfully\n");
                }
                break;
            case EXIT:
                loop_run = false;
                break;
            default:
                printf("enter the right choise\n");
                break;
        }
    }

    exit(0);
}

