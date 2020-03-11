#include "cropping.h"

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
 * @function: to get the yuv raw data file and convert it to a bmp file
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
        if(!(fp = fopen("Cropped_file.bmp", "wb")))
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

        printf("Converted data written successfully in the Cropped_file.bmp\n");

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

/* @authhor: Subhash
 * @function: to get the yuv420P raw data file and crop it
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv420Cropping()
{
    bool function_status = true;
    printf("YUV420 cropping\n");
    return function_status;
}

/* @authhor: Subhash
 * @function: to get the yuv422 raw data file and crop it
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv422Cropping()
{
    bool function_status = true;
    int width, height, pixels, YUV422_filesize, cropped_size, file_size, Y422c_index = 1, row, col, U422_index, U422c_index,
        cropped_width, cropped_height, x_coordinate, y_coordinate, crop_pixel_index, Y422_index = 1;
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
            return false;
        }

    }while(!(fp = fopen(file_name, "rb")));

    if((file_size = fileSize(file_name)) == FAILED)
    {
        printf("Error in fileSize function\n");
        return false;
    }

    while(1)
    {
        printf("Enter the width of the original image\n");
        width = abs(getNumberAndValidate("enter the valid number "));

        printf("Enter the height of the original image\n");
        height = abs(getNumberAndValidate("enter the valid number "));

        pixels = width * height;
        YUV422_filesize = pixels * YUV422_FILESIZE_FACTOR;

        if(file_size != YUV422_filesize)
        {
            goto get_again;
        }

        printf("Enter the x coordinate of the pixel in the image\n");
        x_coordinate = abs(getNumberAndValidate("enter the valid number "));

        printf("Enter the y coordinate of the pixel in the image\n");
        y_coordinate = abs(getNumberAndValidate("enter the valid number "));

        printf("Enter the width to which image to be cropped\n");
        cropped_width = abs(getNumberAndValidate("enter the valid number "));

        printf("Enter the height to which image to be cropped\n");
        cropped_height = abs(getNumberAndValidate("enter the valid number "));

        if(cropped_width > (width - (y_coordinate - 1)))
        {
            cropped_width = width - (y_coordinate - 1);
            printf("Given crop width exceeded the image edge so reduced to %d\n",cropped_width);
        }

        if(cropped_height > (height - (x_coordinate - 1)))
        {
            cropped_height = height - (x_coordinate - 1);
            printf("Given crop height exceeded the image edge so reduced to %d\n",cropped_height);
        }

        cropped_size = cropped_width * cropped_height * YUV422_FILESIZE_FACTOR;
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

    cropped_yuv422_data = (uint8_t *)malloc(cropped_size * sizeof(uint8_t));

    if(!cropped_yuv422_data)
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

    crop_pixel_index = Y422_index +  (x_coordinate - 1) * (2 * width) + (y_coordinate - 1) * 2;

    for(row = crop_pixel_index;row <= crop_pixel_index + (width * 2 * (cropped_height - 1));row += width * 2)
    {
        col = 0;
        for(Y422_index = row;col < cropped_width;col++)
        {
            cropped_yuv422_data[Y422c_index] = raw_yuv422_data[Y422_index];
            Y422_index += 2;
            Y422c_index += 2;
        }
    }

    if(y_coordinate % 2)
    {
        for(row = crop_pixel_index - 1;row <= (crop_pixel_index - 1) + (width * 2 * (cropped_height - 1));row += width * 2)
        {
            col = 0;
            for(U422_index = row;col < cropped_width / 2;col++)
            {
                cropped_yuv422_data[U422c_index] = raw_yuv422_data[U422_index];
                cropped_yuv422_data[U422c_index + 2] = raw_yuv422_data[U422_index + 2];
                U422_index += 4;
                U422c_index += 4;
            }
        }
    }
    else
    {
        for(row = crop_pixel_index - 3;row <= (crop_pixel_index - 1) + (width * 2 * (cropped_height - 1));row += width * 2)
        {
            col = 0;
            for(U422_index = row;col < cropped_width / 2;col++)
            {
                cropped_yuv422_data[U422c_index] = (raw_yuv422_data[U422_index] + raw_yuv422_data[U422_index + 4]) / 2;
                cropped_yuv422_data[U422c_index + 2] = (raw_yuv422_data[U422_index + 2] + raw_yuv422_data[U422_index + 6]) / 2;
                U422_index += 4;
                U422c_index += 4;
            }
        }
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

    if(!yuv422Toyuv444("YUV422_cropped.raw", cropped_width, cropped_height))
    {
        printf(" yuv422Toyuv444 function is not executed successfully\n");
        function_status = false;
    }

    if(!yuvToBmpConversion("YUV444_cropped_file.raw", cropped_width, cropped_height))
    {
        printf(" yuvToBmpConversion function is not executed successfully\n");
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
 * @function: to get the yuv422 raw data file and convert it to yuv444 format
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv422Toyuv444(char *file_name, int width, int height)
{
  FILE *fp = NULL;
  // char *file_name = NULL;
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

  raw_yuv444_data = (uint8_t *)malloc(YUV444_filesize * sizeof(uint8_t));

  if(!raw_yuv444_data)
  {
    printf("memory not allocated\n");
    function_status = false;
    goto QUIT;
  }

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
      if(!(fp = fopen("YUV444_cropped_file.raw", "wb")))
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
int main()
{
    int choise;
    bool loop_run = true;

    while(loop_run)
    {
        printf("\nEnter your choice\
                \n1-- YUV420P_CROPING\
                \n2-- YUV422_CROPING\
                \n3-- Exit\n");
        choise = getNumberAndValidate("enter the right choice ");

        switch(choise)
        {
            case YUV420P_CROP:
                if(!yuv420Cropping())
                {
                    printf(" yuv420Cropping function is not executed successfully\n");
                }
                break;
            case YUV422_CROP:
                if(!yuv422Cropping())
                {
                    printf(" yuv422Cropping function is not executed successfully\n");
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

    exit(1);
}
