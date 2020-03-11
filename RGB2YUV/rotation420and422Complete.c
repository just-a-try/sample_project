#include "rotation420and422Complete.h"

/* @authhor: Subhash
 * @function: to rotate yuv422 raw data to given angle
 * @param int angle: angle to which the yuv422 raw data to be rotated
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv422Rotation(int angle)
{
   FILE *fp = NULL;
   char *file_name = NULL;
   bool function_status = true;
   int width, height, pixels, YUV422_filesize, Y422r_index, Y422_index = 1, rotated_422_index,
       U422_index = 0, file_size, row_index, col_index = 0, uv_width, chroma1_index, chroma2_index;
   uint8_t *raw_yuv422_data = NULL, *rotated_yuv422_data = NULL;

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
       YUV422_filesize = pixels * 2;
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

   rotated_yuv422_data = (uint8_t *)malloc(YUV422_filesize * sizeof(uint8_t));

   if(!rotated_yuv422_data)
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


   if(angle == 180)
   {
       Y422_index = 0;
       U422_index = YUV422_filesize - 4;
       while(Y422_index < YUV422_filesize - 3)
       {
           rotated_yuv422_data[U422_index] = raw_yuv422_data[Y422_index];
           rotated_yuv422_data[U422_index + 1] = raw_yuv422_data[Y422_index + 3];
           rotated_yuv422_data[U422_index + 2] = raw_yuv422_data[Y422_index + 2];
           rotated_yuv422_data[U422_index + 3] = raw_yuv422_data[Y422_index + 1];
           Y422_index += 4;
           U422_index -= 4;
       }

       width = width + height;
       height = width - height;
       width = width - height;
   }
   else if(angle == 90)
   {
       for(row_index = height * 2 - 1;row_index >= 1;row_index -= 2)
       {
           col_index = 0;
           for( Y422r_index= row_index;col_index < width;col_index++)
           {
               rotated_yuv422_data[Y422r_index] = raw_yuv422_data[Y422_index];
               Y422r_index += height * 2;
               Y422_index += 2;
           }
       }
       for(row_index = YUV422_filesize - 2 * width;row_index < YUV422_filesize - 3;row_index += 4)
       {
           col_index = 0;
           if(row_index != YUV422_filesize - 2 * width )
                U422_index += 2 * height;
           for(chroma1_index = row_index;col_index < height / 2;col_index++)
           {
               rotated_yuv422_data[U422_index] = (raw_yuv422_data[chroma1_index] + raw_yuv422_data[chroma1_index - 2 * width]) / 2;
               rotated_yuv422_data[U422_index + 2] = (raw_yuv422_data[chroma1_index + 2] + raw_yuv422_data[chroma1_index - 2 * width + 2]) / 2;
               rotated_yuv422_data[U422_index + 2 * height] =  rotated_yuv422_data[U422_index];
               rotated_yuv422_data[U422_index + 2 * height + 2 ] = rotated_yuv422_data[U422_index + 2];
               chroma1_index -= 4 * width;
               U422_index += 4;

           }
       }
   }
   else
   {
       for(row_index = width * 2 - 1;row_index >= 1;row_index -= 2)
       {
           col_index = 0;
           for( Y422r_index= row_index;col_index < height;col_index++)
           {
               rotated_yuv422_data[Y422_index] = raw_yuv422_data[Y422r_index];
               Y422r_index += width * 2;
               Y422_index += 2;
           }
       }

       U422_index = YUV422_filesize - 4;

       for(row_index = YUV422_filesize - 2 * width;row_index < YUV422_filesize - 3;row_index += 4)
       {
           col_index = 0;
           if(row_index != YUV422_filesize - 2 * width )
                U422_index -= 2 * height;
           for(chroma1_index = row_index;col_index < height / 2;col_index++)
           {
               rotated_yuv422_data[U422_index] = (raw_yuv422_data[chroma1_index] + raw_yuv422_data[chroma1_index - 2 * width]) / 2;
               rotated_yuv422_data[U422_index + 2] = (raw_yuv422_data[chroma1_index + 2] + raw_yuv422_data[chroma1_index - 2 * width + 2]) / 2;
               rotated_yuv422_data[U422_index - 2 * height] =  rotated_yuv422_data[U422_index];
               rotated_yuv422_data[U422_index - 2 * height + 2 ] = rotated_yuv422_data[U422_index + 2];
               chroma1_index -= 4 * width;
               U422_index -= 4;

           }
       }
   }

   if(!fp)
   {
       if(!(fp = fopen("YUV422_rotated.raw", "wb")))
       {
           printf("Error in openning the file\n");
           function_status = false;
           goto QUIT;
       }

       if(fwrite(rotated_yuv422_data, YUV422_filesize, NMEMB, fp) == EOF)
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

   if(rotated_yuv422_data)
       free(rotated_yuv422_data);

   if(raw_yuv422_data)
       free(raw_yuv422_data);

   if(fp)
   {
       if(fclose(fp) == EOF)
       {
           printf("The file is not closed successfully\n");
       }
   }

   if(!yuv422Toyuv444("YUV422_rotated.raw", height, width))
   {
       printf(" yuv420Rotation function is not executed successfully\n");
       function_status = false;
   }

   if(!yuvToBmpConversion("YUV444_converted_file.raw", height, width))
   {
       printf(" yuv420Rotation function is not executed successfully\n");
       function_status = false;
   }

   return function_status;
}


 /* @authhor: Subhash
  * @function: to rotate yuv420 planar raw data to given angle
  * @param int angle: angle to which the yuv420 planar raw data to be rotated
  * @return: returns a true if function executed successfully or false if it fails
  */
bool yuv420Rotation(int angle)
{
    FILE *fp = NULL;
    char *file_name = NULL;
    bool function_status = true;
    int width, height, pixels, YUV420p_filesize, Y420p_index = 0,Y420_index, U420_index,
        V420_index, file_size, row_index, col_index = 0, uv_width, chroma_index;
    uint8_t *raw_yuv420p_data = NULL, *rotated_yuv420p_data = NULL;

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
        uv_width = (width / 2) * (height / 2);
        YUV420p_filesize = pixels + 2 * uv_width;
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

    rotated_yuv420p_data = (uint8_t *)malloc(YUV420p_filesize * sizeof(uint8_t));

    if(!rotated_yuv420p_data)
    {
        printf("memory not allocated\n");
        function_status = false;
        goto QUIT;
    }

    if(angle == 180)
    {
        Y420_index = pixels - 1;
        U420_index = Y420_index + uv_width;
        V420_index = U420_index + uv_width;

        while(Y420p_index < pixels + uv_width)
        {
            if(Y420p_index < pixels)
            {
                rotated_yuv420p_data[Y420_index--] = raw_yuv420p_data[Y420p_index++];
            }
            else
            {
                rotated_yuv420p_data[U420_index--] = raw_yuv420p_data[Y420p_index];
                rotated_yuv420p_data[V420_index--] = raw_yuv420p_data[Y420p_index + uv_width];
                Y420p_index++;
            }
        }
        width = width + height;
        height = width - height;
        width = width - height;
    }
    else if(angle == 90)
    {
        for(row_index = height - 1;row_index >= 0;row_index--)
        {
            col_index = 0;
            for( Y420_index= row_index;col_index < width;col_index++)
            {
                rotated_yuv420p_data[Y420_index] = raw_yuv420p_data[Y420p_index++];
                Y420_index += height;
            }
        }

        for(row_index = pixels + uv_width - (width / 2);row_index < pixels + uv_width;row_index++)
        {
            col_index = 0;
            for(chroma_index = row_index;col_index < height / 2;col_index++)
            {
                rotated_yuv420p_data[Y420p_index] = raw_yuv420p_data[chroma_index];
                rotated_yuv420p_data[Y420p_index + uv_width] = raw_yuv420p_data[chroma_index + uv_width];
                chroma_index -= width / 2;
                Y420p_index++;
            }
        }
    }
    else
    {

        for(row_index = pixels - height;row_index < pixels;row_index++)
        {
            col_index = 0;
            for( Y420_index= row_index;col_index < width;col_index++)
            {
                rotated_yuv420p_data[Y420_index] = raw_yuv420p_data[Y420p_index++];
                Y420_index -= height;
            }
        }

        for(row_index = pixels + uv_width - (height / 2);row_index < pixels + uv_width;row_index++)
        {
            col_index = 0;
            for(chroma_index = row_index;col_index < width / 2;col_index++)
            {
                rotated_yuv420p_data[chroma_index] = raw_yuv420p_data[Y420p_index];
                rotated_yuv420p_data[chroma_index + uv_width] = raw_yuv420p_data[Y420p_index + uv_width];
                chroma_index -= height / 2;
                Y420p_index++;
            }
        }
    }

    if(!fp)
    {
        if(!(fp = fopen("YUV420p_rotated.raw", "wb")))
        {
            printf("Error in openning the file\n");
            function_status = false;
            goto QUIT;
        }

        if(fwrite(rotated_yuv420p_data, YUV420p_filesize, NMEMB, fp) == EOF)
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

    if(rotated_yuv420p_data)
        free(rotated_yuv420p_data);

    if(raw_yuv420p_data)
        free(raw_yuv420p_data);

    if(fp)
    {
        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
        }
    }

    if(!yuv420pToyuv444("YUV420p_rotated.raw", height, width))
    {
        printf(" yuv420Rotation function is not executed successfully\n");
        function_status = false;
    }

    if(!yuvToBmpConversion("YUV444_converted_file.raw", height, width))
    {
        printf(" yuv420Rotation function is not executed successfully\n");
        function_status = false;
    }

    return function_status;
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

    fp = fopen(file_name, "rb");

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
        if(!(fp = fopen("YUV444_converted_file.raw", "wb")))
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

  fp = fopen(file_name, "rb");

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
      if(!(fp = fopen("YUV444_converted_file.raw", "wb")))
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

    fp = fopen(file_name, "rb");

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
        if(!(fp = fopen("newrotatedbmpfile.bmp", "wb")))
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

        printf("Converted data written successfully in the newrotatedbmpfile.bmp\n");

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
    int choise1, choise2, angle;
    bool loop_run = true;

    while(loop_run)
    {
        printf("\nEnter your choice\
                \n1-- YUV420P_ROTATIONS\
                \n2-- YUV422_ROTATIONS\
                \n3-- Exit\n");
        choise1 = getNumberAndValidate("enter the right choice ");

        while(loop_run && choise1!= EXIT)
        {
            printf("\n    ROTATION ANGLES       \
                    \n1-- 90\
                    \n2-- 180\
                    \n3-- 270\n");
            choise2 = getNumberAndValidate("enter the right choice ");
            switch(choise2)
            {
                case ROTATE_90:
                    angle = 90;
                    loop_run = false;
                    break;
                case ROTATE_180:
                    angle = 180;
                    loop_run = false;
                    break;
                case ROTATE_270:
                    angle =270;
                    loop_run = false;
                    break;
                default:
                    printf("enter the right choise\n");
                    loop_run = true;
                    break;
            }
        }
        loop_run = true;
        switch(choise1)
        {
            case YUV420P_ROTATIONS:
                if(!yuv420Rotation(angle))
                {
                    printf(" yuv420Rotation function is not executed successfully\n");
                }
                break;
            case YUV422_ROTATIONS:
                if(!yuv422Rotation(angle))
                {
                    printf(" yuv420Rotation function is not executed successfully\n");
                }
                break;
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
