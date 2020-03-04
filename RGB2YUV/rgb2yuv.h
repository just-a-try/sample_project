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
 * @function: to get the yuv420 planar raw data file and convert it to yuv422 Planar format
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv420pToyuv422()
{
    FILE*fp;
    char *file_name;
    bool get_again;
    int flag = 0, width, height, pixels, YUV420p_filesize, YUV422_filesize,
        Y420p_index = 0, Y422_index = 1, U422_index = 0, V422_index = 2;
    uint8_t *raw_yuv420p_data, *raw_yuv422_data;

    do{
        get_again = false;
        printf("Enter the YUV422 raw file name\n");
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

    printf("Enter the width of the image\n");
    width = abs(getNumberAndValidate("enter the valid number "));

    printf("Enter the height of the image\n");
    height = abs(getNumberAndValidate("enter the valid number "));

    pixels = width * height;
    YUV422_filesize = pixels * BYTES_IN_PIXEL - pixels;
    YUV420p_filesize = pixels + 2 * ((width / 2) * (height / 2));

    raw_yuv420p_data = malloc(YUV420p_filesize);

    if(!raw_yuv420p_data)
    {
        printf("memory not allocated\n");
        goto QUIT;
    }

    if(!fread(raw_yuv420p_data, YUV420p_filesize, NMEMB, fp))
    {
       printf("fread is not excecuted successfully\n");
       goto QUIT;
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
        file_name ? free(file_name) : printf("pointer is null cant be freed\n");
        raw_yuv420p_data ? free(raw_yuv420p_data) : printf("pointer is null cant be freed\n");
        return false;
    }

    fp = NULL;

    raw_yuv422_data = malloc(YUV422_filesize);

    if(!raw_yuv422_data)
    {
        printf("memory not allocated\n");
        goto QUIT;
    }

    while(Y420p_index < YUV420p_filesize)
    {
        if(Y420p_index < pixels)
        {
            raw_yuv422_data[Y422_index] = raw_yuv420p_data[Y420p_index];
            Y422_index += 2;
            Y420p_index++;
        }
        else if(Y420p_index < pixels + ((width / 2) * (height / 2)))
        {
            raw_yuv422_data[U422_index] = raw_yuv422_data[U422_index + 2 * width] =
                                          raw_yuv420p_data[Y420p_index];
            flag += 2;

            if(flag == width)
            {
                U422_index += 2 * width + 4;
                flag = 0;
            }
            else
            {
                U422_index += 4;
            }

            Y420p_index++;
        }
        else
        {
            raw_yuv422_data[V422_index] = raw_yuv422_data[V422_index + 2 * width] =
                                          raw_yuv420p_data[Y420p_index];
            flag += 2;

            if(flag == width)
            {
                V422_index += 2 * width + 4;
                flag = 0;
            }
            else
            {
                V422_index += 4;
            }

            Y420p_index++;

        }
    }

    if(!fp)
    {
        if(!(fp = fopen("YUV422_from_YUV420P_file.raw", "wb")))
        {
            printf("Error in openning the file\n");
            goto QUIT;
        }

        if(fwrite(raw_yuv422_data, YUV422_filesize, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            goto QUIT;
        }

        printf("Converted data written successfully in the file YUV420_from_YUV422_file.raw\n");

        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
            file_name ? free(file_name) : printf("pointer is null cant be freed\n");
            raw_yuv422_data ? free(raw_yuv422_data) : printf("pointer is null cant be freed\n");
            raw_yuv420p_data ? free(raw_yuv420p_data) : printf("pointer is null cant be freed\n");
            return false;
        }
    }
    else
    {
        printf("file pointer is not null\n");
    }

    file_name ? free(file_name) : printf("pointer is null cant be freed\n");
    raw_yuv422_data ? free(raw_yuv422_data) : printf("pointer is null cant be freed\n");
    raw_yuv420p_data ? free(raw_yuv420p_data) : printf("pointer is null cant be freed\n");
    return true;

    QUIT:

    file_name ? free(file_name) : printf("pointer is null cant be freed\n");
    raw_yuv422_data ? free(raw_yuv422_data) : printf("pointer is null cant be freed\n");
    raw_yuv420p_data ? free(raw_yuv420p_data) : printf("pointer is null cant be freed\n");

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
    }

    return false;
}

/* @authhor: Subhash
 * @function: to get the yuv422 raw data file and convert it to yuv420 Planar format
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv422Toyuv420p()
{
    FILE*fp;
    char *file_name;
    bool get_again;
    int flag = 0, width, height, pixels, YUV420p_filesize, YUV422_filesize,
        Y420p_index = 0, Y422_index = 1, U422_index = 0, V422_index = 2;
    uint8_t *raw_yuv420p_data, *raw_yuv422_data;

    do{
        get_again = false;
        printf("Enter the YUV422 raw file name\n");
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

    printf("Enter the width of the image\n");
    width = abs(getNumberAndValidate("enter the valid number "));

    printf("Enter the height of the image\n");
    height = abs(getNumberAndValidate("enter the valid number "));

    pixels = width * height;
    YUV422_filesize = pixels * BYTES_IN_PIXEL - pixels;
    YUV420p_filesize = pixels + 2 * ((width / 2) * (height / 2));

    raw_yuv422_data = malloc(YUV422_filesize);

    if(!raw_yuv422_data)
    {
        printf("memory not allocated\n");
        goto QUIT;
    }

    if(!fread(raw_yuv422_data, YUV422_filesize, NMEMB, fp))
    {
       printf("fread is not excecuted successfully\n");
       goto QUIT;
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
        file_name ? free(file_name) : printf("pointer is null cant be freed\n");
        raw_yuv422_data ? free(raw_yuv422_data) : printf("pointer is null cant be freed\n");
        return false;
    }

    fp = NULL;

    raw_yuv420p_data = malloc(YUV420p_filesize);

    if(!raw_yuv420p_data)
    {
        printf("memory not allocated\n");
        goto QUIT;
    }

    while(Y420p_index < YUV420p_filesize)
    {
        if(Y420p_index < pixels)
        {
            raw_yuv420p_data[Y420p_index] = raw_yuv422_data[Y422_index];
            Y422_index += 2;
            Y420p_index++;
        }
        else if(Y420p_index < pixels + ((width / 2) * (height / 2)))
        {
            raw_yuv420p_data[Y420p_index]  = (raw_yuv422_data[U422_index] + raw_yuv422_data[U422_index + 2 * width]) / 2;
            flag += 2;

            if(flag == width)
            {
                U422_index += 2 * width + 4;
                flag = 0;
            }
            else
            {
                U422_index += 4;
            }

            Y420p_index++;
        }
        else
        {
            raw_yuv420p_data[Y420p_index]  = (raw_yuv422_data[V422_index] + raw_yuv422_data[V422_index + 2 * width]) / 2;
            flag += 2;

            if(flag == width)
            {
                V422_index += 2 * width + 4;
                flag = 0;
            }
            else
            {
                V422_index += 4;
            }

            Y420p_index++;

        }
    }

    if(!fp)
    {
        if(!(fp = fopen("YUV420_from_YUV422_file.raw", "wb")))
        {
            printf("Error in openning the file\n");
            goto QUIT;
        }

        if(fwrite(raw_yuv420p_data, YUV420p_filesize, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            goto QUIT;
        }

        printf("Converted data written successfully in the file YUV420_from_YUV422_file.raw\n");

        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
            file_name ? free(file_name) : printf("pointer is null cant be freed\n");
            raw_yuv422_data ? free(raw_yuv422_data) : printf("pointer is null cant be freed\n");
            raw_yuv420p_data ? free(raw_yuv420p_data) : printf("pointer is null cant be freed\n");
            return false;
        }
    }
    else
    {
        printf("file pointer is not null\n");
    }

    file_name ? free(file_name) : printf("pointer is null cant be freed\n");
    raw_yuv422_data ? free(raw_yuv422_data) : printf("pointer is null cant be freed\n");
    raw_yuv420p_data ? free(raw_yuv420p_data) : printf("pointer is null cant be freed\n");
    return true;

    QUIT:

    file_name ? free(file_name) : printf("pointer is null cant be freed\n");
    raw_yuv422_data ? free(raw_yuv422_data) : printf("pointer is null cant be freed\n");
    raw_yuv420p_data ? free(raw_yuv420p_data) : printf("pointer is null cant be freed\n");

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
    }

    return false;
}

/* @authhor: Subhash
 * @function: to get the yuv420P raw data file and convert it to yuv444 format
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv420pToyuv444()
{
    FILE*fp;
    char *file_name;
    bool get_again;
    int flag = 0, width, height, pixels, YUV420p_filesize, YUV444_filesize,
        Y420p_index = 0, Y444_index = 0, U444_index = 1, V444_index = 2;
    uint8_t *raw_yuv420p_data, *raw_yuv444_data;

    do{
        get_again = false;
        printf("Enter the YUV420p raw file name\n");
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

    printf("Enter the width of the image\n");
    width = abs(getNumberAndValidate("enter the valid number "));

    printf("Enter the height of the image\n");
    height = abs(getNumberAndValidate("enter the valid number "));

    pixels = width * height;
    YUV444_filesize = pixels * BYTES_IN_PIXEL;
    YUV420p_filesize = pixels + 2 * ((width / 2) * (height / 2));

    raw_yuv420p_data = malloc(YUV420p_filesize);

    if(!raw_yuv420p_data)
    {
        printf("memory not allocated\n");
        goto QUIT;
    }

    if(!fread(raw_yuv420p_data, YUV420p_filesize, NMEMB, fp))
    {
       printf("fread is not excecuted successfully\n");
       goto QUIT;
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
        file_name ? free(file_name) : printf("pointer is null cant be freed\n");
        raw_yuv420p_data ? free(raw_yuv420p_data) : printf("pointer is null cant be freed\n");
        return false;
    }

    fp = NULL;

    raw_yuv444_data = malloc(YUV444_filesize);

    if(!raw_yuv444_data)
    {
        printf("memory not allocated\n");
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
        if(!(fp = fopen("YUV444_from_420p_file.raw", "wb")))
        {
            printf("Error in openning the file\n");
            goto QUIT;
        }

        if(fwrite(raw_yuv444_data, YUV444_filesize, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            goto QUIT;
        }

        printf("Converted data written successfully in the file YUV444_from_420p_file.raw\n");

        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
            file_name ? free(file_name) : printf("pointer is null cant be freed\n");
            raw_yuv444_data ? free(raw_yuv444_data) : printf("pointer is null cant be freed\n");
            raw_yuv420p_data ? free(raw_yuv420p_data) : printf("pointer is null cant be freed\n");
            return false;
        }
    }
    else
    {
        printf("file pointer is not null\n");
    }

    file_name ? free(file_name) : printf("pointer is null cant be freed\n");
    raw_yuv444_data ? free(raw_yuv444_data) : printf("pointer is null cant be freed\n");
    raw_yuv420p_data ? free(raw_yuv420p_data) : printf("pointer is null cant be freed\n");
    return true;

    QUIT:

    file_name ? free(file_name) : printf("pointer is null cant be freed\n");
    raw_yuv444_data ? free(raw_yuv444_data) : printf("pointer is null cant be freed\n");
    raw_yuv420p_data ? free(raw_yuv420p_data) : printf("pointer is null cant be freed\n");

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
    }

    return false;


}

/* @authhor: Subhash
 * @function: to get the yuv444 raw data file and convert it to yuv420 plannar format
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv444Toyuv420p()
{
  FILE*fp;
  char *file_name;
  bool get_again;
  int flag = 0, width, height, pixels, YUV420p_filesize, YUV444_filesize,
      Y420p_index = 0, Y444_index = 0, U444_index = 1, V444_index = 2;
  uint8_t *raw_yuv420p_data, *raw_yuv444_data;

  do{
      get_again = false;
      printf("Enter the YUV444 raw file name\n");
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

  printf("Enter the width of the image\n");
  width = abs(getNumberAndValidate("enter the valid number "));

  printf("Enter the height of the image\n");
  height = abs(getNumberAndValidate("enter the valid number "));

  pixels = width * height;
  YUV444_filesize = pixels * BYTES_IN_PIXEL;
  YUV420p_filesize = pixels + 2 * ((width / 2) * (height / 2));

  raw_yuv444_data = malloc(YUV444_filesize);

  if(!raw_yuv444_data)
  {
    printf("memory not allocated\n");
    goto QUIT;
  }

  if(!fread(raw_yuv444_data, YUV444_filesize, NMEMB, fp))
  {
     printf("fread is not excecuted successfully\n");
     goto QUIT;
  }

  if(fclose(fp) == EOF)
  {
      printf("The file is not closed successfully\n");
      file_name ? free(file_name) : printf("pointer is null cant be freed\n");
      raw_yuv444_data ? free(raw_yuv444_data) : printf("pointer is null cant be freed\n");
      return false;
  }

  fp = NULL;

  raw_yuv420p_data = malloc(YUV420p_filesize);

  if(!raw_yuv420p_data)
  {
      printf("memory not allocated\n");
      goto QUIT;
  }

  while(Y420p_index < YUV420p_filesize)
  {
        if(Y420p_index < pixels)
        {
            raw_yuv420p_data[Y420p_index] = raw_yuv444_data[Y444_index];
            Y444_index += 3;
            Y420p_index++;
        }
        else if(Y420p_index < pixels + ((width / 2) * (height / 2)))
        {
            raw_yuv420p_data[Y420p_index]  = (raw_yuv444_data[U444_index] +
                                              raw_yuv444_data[U444_index + 3] +
                                              raw_yuv444_data[U444_index +
                                              BYTES_IN_PIXEL * width] +
                                              raw_yuv444_data[U444_index +
                                              BYTES_IN_PIXEL * width + 3]) / 4;
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
            raw_yuv420p_data[Y420p_index] = (raw_yuv444_data[V444_index] +
                                             raw_yuv444_data[V444_index + 3] +
                                             raw_yuv444_data[V444_index +
                                             BYTES_IN_PIXEL * width] +
                                             raw_yuv444_data[V444_index +
                                             BYTES_IN_PIXEL * width + 3]) / 4;
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
      if(!(fp = fopen("YUV420_file.raw", "wb")))
      {
          printf("Error in openning the file\n");
          goto QUIT;
      }

      if(fwrite(raw_yuv420p_data, YUV420p_filesize, NMEMB, fp) == EOF)
      {
          printf("fwrite is not excecuted successfully\n");
          goto QUIT;
      }

      printf("Converted data written successfully in the file YUV420_file.raw\n");

      if(fclose(fp) == EOF)
      {
          printf("The file is not closed successfully\n");
          file_name ? free(file_name) : printf("pointer is null cant be freed\n");
          raw_yuv444_data ? free(raw_yuv444_data) : printf("pointer is null cant be freed\n");
          raw_yuv420p_data ? free(raw_yuv420p_data) : printf("pointer is null cant be freed\n");
          return false;
      }
  }
  else
  {
      printf("file pointer is not null\n");
  }

  file_name ? free(file_name) : printf("pointer is null cant be freed\n");
  raw_yuv444_data ? free(raw_yuv444_data) : printf("pointer is null cant be freed\n");
  raw_yuv420p_data ? free(raw_yuv420p_data) : printf("pointer is null cant be freed\n");
  return true;

  QUIT:

  file_name ? free(file_name) : printf("pointer is null cant be freed\n");
  raw_yuv444_data ? free(raw_yuv444_data) : printf("pointer is null cant be freed\n");
  raw_yuv420p_data ? free(raw_yuv420p_data) : printf("pointer is null cant be freed\n");

  if(fclose(fp) == EOF)
  {
      printf("The file is not closed successfully\n");
  }

  return false;



}
/* @authhor: Subhash
 * @function: to get the yuv422 raw data file and convert it to yuv444 format
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv422Toyuv444()
{
  FILE*fp;
  char *file_name;
  bool get_again = false;
  int width, height, YUV444_filesize, YUV422_filesize, Y444_index = 0, Y422_index = 0;
  uint8_t *raw_yuv444_data, *raw_yuv422_data;

  do{
      printf("Enter the YUV444 raw file name\n");
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

  printf("Enter the width of the image\n");
  width = abs(getNumberAndValidate("enter the valid number "));

  printf("Enter the height of the image\n");
  height = abs(getNumberAndValidate("enter the valid number "));

  YUV444_filesize = width * height * BYTES_IN_PIXEL;
  YUV422_filesize = YUV444_filesize - width * height;

  raw_yuv422_data = malloc(YUV422_filesize);

  if(!raw_yuv422_data)
  {
    printf("memory not allocated\n");
    goto QUIT;
  }

  if(!fread(raw_yuv422_data, YUV422_filesize, NMEMB, fp))
  {
     printf("fread is not excecuted successfully\n");
     goto QUIT;
  }

  if(fclose(fp) == EOF)
  {
      printf("The file is not closed successfully\n");
      file_name ? free(file_name) : printf("pointer is null cant be freed\n");
      raw_yuv422_data ? free(raw_yuv422_data) : printf("pointer is null cant be freed\n");
      return false;
  }

  fp = NULL;

  raw_yuv444_data = malloc(YUV444_filesize);

  if(!raw_yuv444_data)
  {
    printf("memory not allocated\n");
    goto QUIT;
  }

  while(Y422_index < YUV422_filesize - 3)
  {
    raw_yuv444_data[Y444_index] = raw_yuv422_data[Y422_index + 1];
    raw_yuv444_data[Y444_index + 1] = raw_yuv422_data[Y422_index];
    raw_yuv444_data[Y444_index + 2] = raw_yuv422_data[Y422_index + 2];
    raw_yuv444_data[Y444_index + 3] = raw_yuv422_data[Y422_index + 3];
    raw_yuv444_data[Y444_index + 4] = raw_yuv422_data[Y422_index];
    raw_yuv444_data[Y444_index + 5] = raw_yuv422_data[Y422_index + 2];
    Y422_index += 4;
    Y444_index += 6;
  }

  if(!fp)
  {
      if(!(fp = fopen("newYUV444_file.raw", "wb")))
      {
          printf("Error in openning the file\n");
          goto QUIT;
      }

      if(fwrite(raw_yuv444_data, YUV444_filesize, NMEMB, fp) == EOF)
      {
          printf("fwrite is not excecuted successfully\n");
          goto QUIT;
      }

      printf("Converted data written successfully in the file newYUV444_file.raw\n");

      if(fclose(fp) == EOF)
      {
          printf("The file is not closed successfully\n");
          file_name ? free(file_name) : printf("pointer is null cant be freed\n");
          raw_yuv444_data ? free(raw_yuv444_data) : printf("pointer is null cant be freed\n");
          raw_yuv422_data ? free(raw_yuv422_data) : printf("pointer is null cant be freed\n");
          return false;
      }
  }
  else
  {
      printf("file pointer is not null\n");
  }

  file_name ? free(file_name) : printf("pointer is null cant be freed\n");
  raw_yuv444_data ? free(raw_yuv444_data) : printf("pointer is null cant be freed\n");
  raw_yuv422_data ? free(raw_yuv422_data) : printf("pointer is null cant be freed\n");
  return true;

  QUIT:

  file_name ? free(file_name) : printf("pointer is null cant be freed\n");
  raw_yuv444_data ? free(raw_yuv444_data) : printf("pointer is null cant be freed\n");
  raw_yuv422_data ? free(raw_yuv422_data) : printf("pointer is null cant be freed\n");

  if(fclose(fp) == EOF)
  {
      printf("The file is not closed successfully\n");
  }

  return false;
}
/* @authhor: Subhash
 * @function: to get the yuv444 raw data file and convert it to yuv422 format
 * @return: returns a true if function executed successfully or false if it fails
 */
bool yuv444Toyuv422()
{
    FILE*fp;
    char *file_name;
    bool get_again = false;
    int width, height, YUV444_filesize, YUV422_filesize, Y444_index = 0, Y422_index = 0;
    uint8_t *raw_yuv444_data, *raw_yuv422_data;

    do{
        printf("Enter the YUV444 raw file name\n");
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

    printf("Enter the width of the image\n");
    width = abs(getNumberAndValidate("enter the valid number "));

    printf("Enter the height of the image\n");
    height = abs(getNumberAndValidate("enter the valid number "));

    YUV444_filesize = width * height * BYTES_IN_PIXEL;

    raw_yuv444_data = malloc(YUV444_filesize);

    if(!raw_yuv444_data)
    {
        printf("memory not allocated\n");
        goto QUIT;
    }

    if(!fread(raw_yuv444_data, YUV444_filesize, NMEMB, fp))
    {
       printf("fread is not excecuted successfully\n");
       goto QUIT;
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");
        file_name ? free(file_name) : printf("pointer is null cant be freed\n");
        raw_yuv444_data ? free(raw_yuv444_data) : printf("pointer is null cant be freed\n");
        return false;
    }

    fp = NULL;
    YUV422_filesize = YUV444_filesize - width * height;

    raw_yuv422_data = malloc(YUV422_filesize);

    if(!raw_yuv422_data)
    {
        printf("memory not allocated\n");
        goto QUIT;
    }

    while(Y444_index < (YUV444_filesize - YUV444_INDEX_LIMIT))
    {
        raw_yuv422_data[Y422_index] = (raw_yuv444_data[Y444_index + FIRST_U_INDEX] +
                                       raw_yuv444_data[Y444_index + SECOND_U_INDEX]) / 2;
        raw_yuv422_data[Y422_index + 1] = raw_yuv444_data[Y444_index];
        raw_yuv422_data[Y422_index + 2] =(raw_yuv444_data[Y444_index + FIRST_V_INDEX] +
                                          raw_yuv444_data[Y444_index + SECOND_V_INDEX]) / 2;
        raw_yuv422_data[Y422_index + 3] = raw_yuv444_data[Y444_index + 3];
        Y422_index += 4;
        Y444_index += 6;
    }

    if(!fp)
    {
        if(!(fp = fopen("YUV422_file.raw", "wb")))
        {
            printf("Error in openning the file\n");
            goto QUIT;
        }

        if(fwrite(raw_yuv422_data, YUV422_filesize, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            goto QUIT;
        }

        printf("Converted data written successfully in the file YUV422_file.raw\n");

        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");
            file_name ? free(file_name) : printf("pointer is null cant be freed\n");
            raw_yuv444_data ? free(raw_yuv444_data) : printf("pointer is null cant be freed\n");
            raw_yuv422_data ? free(raw_yuv422_data) : printf("pointer is null cant be freed\n");
            return false;
        }
    }
    else
    {
        printf("file pointer is not null\n");
    }

    file_name ? free(file_name) : printf("pointer is null cant be freed\n");
    raw_yuv444_data ? free(raw_yuv444_data) : printf("pointer is null cant be freed\n");
    raw_yuv422_data ? free(raw_yuv422_data) : printf("pointer is null cant be freed\n");
    return true;

    QUIT:

    file_name ? free(file_name) : printf("pointer is null cant be freed\n");
    raw_yuv444_data ? free(raw_yuv444_data) : printf("pointer is null cant be freed\n");
    raw_yuv422_data ? free(raw_yuv422_data) : printf("pointer is null cant be freed\n");

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

    file_name ? free(file_name) : printf("pointer is null cant be freed\n");
    raw_yuv_data ? free(raw_yuv_data) : printf("pointer is null cant be freed\n");
    raw_rgb_data ? free(raw_rgb_data) : printf("pointer is null cant be freed\n");
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
                \n1-- BMP to YUV444\
                \n2-- YUV444 to BMP\
                \n3-- YUV444 to YUV422\
                \n4-- YUV422 to YUV444\
                \n5-- YUV444 to YUV420P\
                \n6-- YUV420P to YUV444\
                \n7-- YUV422 to YUV420p\
                \n8-- YUV420P to YUV422\
                \n9-- Exit\n");
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
                break;
            case YUV444TOYUV422:
                if(!yuv444Toyuv422())
                {
                    printf("yuv444Toyuv422 function is not executed successfully\n");
                }
                break;
            case YUV422TOYUV444:
                if(!yuv422Toyuv444())
                {
                    printf("yuv422Toyuv444 function is not executed successfully\n");
                }
                break;
            case YUV444TOYUV420P:
                if(!yuv444Toyuv420p())
                {
                  printf("yuv444Toyuv420p function is not executed successfully\n");
                }
                break;
            case YUV420PTOYUV444:
                if(!yuv420pToyuv444())
                {
                  printf("yuv444Toyuv420p function is not executed successfully\n");
                }
                break;
            case YUV422TOYUV420P:
                if(!yuv422Toyuv420p())
                {
                    printf("yuv422Toyuv420p function is not executed successfully\n");
                }
                break;
            case YUV420PTOYUV422:
                if(!yuv420pToyuv422())
                {
                    printf("yuv420pToyuv422 function is not executed successfully\n");
                }
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

