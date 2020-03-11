#include "zooming.h"


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
    // char *file_name = NULL;
    bool function_status = true;
    int num_of_bytes2pad, YUV444_filesize, raw_yuv_index = 0, raw_rgb_index, pixel_per_row = 1;
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
        if(!(fp = fopen("Zoomedfile.bmp", "wb")))
        {
            printf("Error in openning the file\n");
            function_status = false;
            goto QUIT;
        }

        if(fwrite(&bmpName, FORMAT_NAME_SIZE, NMEMB, fp) == EOF)
        {
            printf("fwrite 1 is not excecuted successfully\n");
            function_status = false;
            goto QUIT;
        }

        if(fwrite(&bmpHeader, BMP_HEADER_SIZE, NMEMB, fp) == EOF)
        {
            printf("fwrite 2 is not excecuted successfully\n");
            function_status = false;
            goto QUIT;
        }

        if(fwrite(&dibHeader, DIB_HEADER_SIZE, NMEMB, fp) == EOF)
        {
            printf("fwrite 3 is not excecuted successfully\n");
            function_status = false;
            goto QUIT;
        }

        if(fwrite(raw_rgb_data, dibHeader.bmp_bytesz, NMEMB, fp) == EOF)
        {
            printf("fwrite 4 is not excecuted successfully\n");
            function_status = false;
            goto QUIT;
        }

        printf("Converted data written successfully in the Zoomedfile.bmp\n");

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
 * @function: to get the yuv422 raw data file and convert it to yuv444 format
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv422Toyuv444(char *file_name, int width, int height)
{
  FILE *fp = NULL;
  // char *file_name = NULL;
  bool function_status = true;
  int file_size, YUV444_filesize, YUV422_filesize, Y444_index = 0, Y422_index = 0;
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
      if(!(fp = fopen("YUV444_zoomed_file.raw", "wb")))
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

      // printf("Converted data written successfully in the file YUV444_from_422_file.raw\n");

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
 * @function: to get the yuv420P raw data file and convert it to yuv444 format
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

    raw_yuv444_data = (uint8_t *)malloc(YUV444_filesize * sizeof(uint8_t));

    if(!raw_yuv444_data)
    {
        printf("memory not allocated\n");
        function_status = false;
        goto QUIT;
    }

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
        if(!(fp = fopen("YUV444_zoomed_file.raw", "wb")))
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

    //    printf("Converted data written successfully in the file YUV444_from_420pRotated_file.raw\n");

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
 * @function: to resize the yuv420p file with given zoomfactor
 * @param integer zoom_factor: zoom factor to which the image to be resized
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv420Zooming(int zoom_factor)
{
    bool function_status = true;
    int width, height, pixels, YUV420p_filesize, zoom_file_size, file_size, Y420_index,
        row, col, flag1, flag2, Y420z_index = 0;
    uint8_t *raw_yuv420p_data = NULL, *zoomed_yuv420p_data = NULL;
    FILE *fp = NULL;
    char *file_name = NULL;

    do{

        if(file_name)
        {
            printf("Given raw file is not present, give a correct file name\n");

            if(file_name)
                free(file_name);
        }

        printf("Enter the YUV420p raw file name\n");
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
        printf("Enter the width of the image\n");
        width = abs(getNumberAndValidate("enter the valid number "));

        printf("Enter the height of the image\n");
        height = abs(getNumberAndValidate("enter the valid number "));

        pixels = width * height;
        YUV420p_filesize = pixels + 2 * (width / 2) * (height / 2);
        zoom_file_size = (width * zoom_factor) * (height * zoom_factor) * YUV420_FILESIZE_FACTOR;

        if(file_size == YUV420p_filesize)
        {
            break;
        }

        printf("Given size does not match with original file size please give correct size\n");
    }

    raw_yuv420p_data = (uint8_t *)malloc(YUV420p_filesize * sizeof(uint8_t));

    if(!raw_yuv420p_data)
    {
        printf("memory not allocated\n");
        function_status = false;
        goto QUIT;
    }

    zoomed_yuv420p_data = (uint8_t *)malloc(zoom_file_size * sizeof(uint8_t));

    if(!zoom_file_size)
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

    for(row = 0;row <= pixels - width;row += width)
    {
        for(flag1 = 0;flag1 < zoom_factor;flag1++)
        {
            for(Y420_index = row,col = 0;col < width;col++,Y420_index++)
            {
                for(flag2 = 0;flag2 < zoom_factor;flag2++)
                {
                    zoomed_yuv420p_data[Y420z_index++] = raw_yuv420p_data[Y420_index];
                }
            }
        }
    }

    for(row = pixels;row <= YUV420p_filesize - width / 2;row += width / 2)
    {
        for(flag1 = 0;flag1 < zoom_factor;flag1++)
        {
            for(Y420_index = row,col = 0;col < width / 2;col++,Y420_index++)
            {
                for(flag2 = 0;flag2 < zoom_factor;flag2++)
                {
                    zoomed_yuv420p_data[Y420z_index++] = raw_yuv420p_data[Y420_index];
                }
            }
        }
    }

    if(!fp)
    {
        if(!(fp = fopen("YUV420p_zoomed.raw", "wb")))
        {
            printf("Error in openning the file\n");
            function_status = false;
            goto QUIT;
        }

        if(fwrite(zoomed_yuv420p_data, zoom_file_size, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            function_status = false;
            goto QUIT;
        }

        //printf("Converted data written successfully in the file YUV420p_rotated.raw\n");

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

    if(file_name)
        free(file_name);

    if(zoomed_yuv420p_data)
        free(zoomed_yuv420p_data);

    if(raw_yuv420p_data)
        free(raw_yuv420p_data);

    if(fp)
    {
        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
        }
    }

    width *= zoom_factor;
    height *= zoom_factor;

    if(!yuv420pToyuv444("YUV420p_zoomed.raw", width, height))
    {
        printf(" yuv420Rotation function is not executed successfully\n");
        function_status = false;
    }

    if(!yuvToBmpConversion("YUV444_zoomed_file.raw", width, height))
    {
        printf(" yuv420Rotation function is not executed successfully\n");
        function_status = false;
    }

    return function_status;
}


bool yuv422Zooming(int zoom_factor)
{
    bool function_status = true;
    int width, height, pixels, YUV422_filesize, zoom_file_size, file_size, Y422_index,
        row, col, flag1, flag2, Y422z_index;
    uint8_t *raw_yuv422_data = NULL, *zoomed_yuv422_data = NULL;
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
        printf("Enter the width of the image\n");
        width = abs(getNumberAndValidate("enter the valid number "));

        printf("Enter the height of the image\n");
        height = abs(getNumberAndValidate("enter the valid number "));

        pixels = width * height;
        YUV422_filesize = pixels * YUV422_FILESIZE_FACTOR;
        zoom_file_size = (width * zoom_factor) * (height * zoom_factor)* YUV422_FILESIZE_FACTOR;

        if(file_size == YUV422_filesize)
        {
            break;
        }

        printf("Given size does not match with original file size please give correct size\n");
    }

    raw_yuv422_data = (uint8_t *)malloc(YUV422_filesize * sizeof(uint8_t));

    if(!raw_yuv422_data)
    {
        printf("memory not allocated\n");
        function_status = false;
        goto QUIT;
    }

    zoomed_yuv422_data = (uint8_t *)malloc(zoom_file_size * sizeof(uint8_t));

    if(!zoom_file_size)
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

        //printf("Converted data written successfully in the file YUV420p_rotated.raw\n");

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

    if(file_name)
        free(file_name);

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

    width *= zoom_factor;
    height *= zoom_factor;

    if(!yuv422Toyuv444("YUV422_zoomed.raw", width, height))
    {
        printf(" yuv420Rotation function is not executed successfully\n");
        function_status = false;
    }

    if(!yuvToBmpConversion("YUV444_zoomed_file.raw", width, height))
    {
        printf(" yuv420Rotation function is not executed successfully\n");
        function_status = false;
    }

    return function_status;
}

int main()
{
    int choise, zoom_factor;
    bool loop_run = true;

    while(loop_run)
    {
        printf("\nEnter your choice\
                \n1-- YUV420P_ZOOMING\
                \n2-- YUV422_ZOOMING\
                \n3-- Exit\n");
        choise = getNumberAndValidate("enter the right choice ");

        while(choise != EXIT)
        {
            printf("\n       IMAGE ZOOMING        \
                  \nEnter the zoom factor from 2x to 10x\n");
            zoom_factor = getNumberAndValidate("enter the right number \n");

            if(zoom_factor > 1 && zoom_factor < 11)
                break;
        }

        switch(choise)
        {
            case YUV420P_ZOOMING:
                if(!yuv420Zooming(zoom_factor))
                {
                    printf(" yuv420Rotation function is not executed successfully\n");
                }
                break;
            case YUV422_ZOOMING:
                if(!yuv422Zooming(zoom_factor))
                {
                    printf(" yuv420Rotation function is not executed successfully\n");
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
