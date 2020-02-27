#include"rgb2yuv.h"

int main()
{
    FILE*fp;
    int num_of_padding, raw_rgb_index = 0;
    uint8_t *raw_rgb_data, y, u, v;

    if(!(fp = fopen("original.bmp", "rb")))
    {
        printf("Error in openning the file\n");
        exit(1);
    }

    if(!fread(&bmpName, FORMAT_NAME_SIZE, NMEMB, fp))
    {
       printf("fread is not excecuted successfully\n");
       goto QUIT;
    }

    if(!fread(&bmpHeader, BMP_HEADER_SIZE, NMEMB, fp))
    {
       printf("fread is not excecuted successfully\n");
       goto QUIT;
    }

    if(!fread(&dibHeader, DIB_HEADER_SIZE, NMEMB, fp))
    {
       printf("fread is not excecuted successfully\n");
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
       printf("fread is not excecuted successfully\n");
       goto QUIT;
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is not closed successfully\n");

        if(!raw_rgb_data)
        {
            printf("raw_rgb_data pointer is null can't be freed");
        }

        free(raw_rgb_data);
        exit(1);
    }
    fp = NULL;
    // num_of_padding = dibHeader.bmp_bytesz - ((-dibHeader.height) * dibHeader.width * BYTES_IN_PIXEL);
    // printf("pad size %d\n",num_of_padding);//(bmpHeader.height * bmpHeader.width - 2)

/* Logic for RGB to YUV conversion :
 * First 3 bytes of the RGB array taken and converted to Y, U, V separately and
 * stored in the same array with same index position where RGB data is taken
 */
    while(raw_rgb_index < -dibHeader.height * dibHeader.width * BYTES_IN_PIXEL - 2)
    {
        y = 0.114 * raw_rgb_data[raw_rgb_index] + 0.587 * raw_rgb_data[raw_rgb_index + 1] + 0.299 * raw_rgb_data[raw_rgb_index+2];
        u = 0.492 * (raw_rgb_data[raw_rgb_index] - y);
        v = 0.877 * (raw_rgb_data[raw_rgb_index+2] - y);
        raw_rgb_data[raw_rgb_index] = y;
        raw_rgb_data[raw_rgb_index + 1] = u+128;
        raw_rgb_data[raw_rgb_index + 2] = v+128;
        raw_rgb_index += BYTES_IN_PIXEL;
    }

    if(!fp)
    {
        if(!(fp = fopen("YUV_file.raw", "wb")))
        {
            printf("Error in openning the file\n");
            goto QUIT;
        }

        if(fwrite(raw_rgb_data, dibHeader.bmp_bytesz, NMEMB, fp) == EOF)
        {
            printf("fwrite is not excecuted successfully\n");
            goto QUIT;
        }

        printf("Converted data written successfully in the file YUV_file.raw\n");

        if(fclose(fp) == EOF)
        {
            printf("The file is not closed successfully\n");

            if(!raw_rgb_data)
            {
                printf("raw_rgb_data pointer is null can't be freed");
            }

            free(raw_rgb_data);
            exit(1);
        }
    }
    else
    {
        printf("file pointer is not null\n");
    }


    if(!raw_rgb_data)
    {
        printf("raw_rgb_data pointer is null can't be freed");
    }

    free(raw_rgb_data);
    exit(0);

    QUIT:

    if(raw_rgb_data)
    {
        free(raw_rgb_data);
    }
    else
    {
        printf("raw_rgb_data pointer is null can't be freed");
    }

    if(fclose(fp) == EOF)
    {
        printf("The file is n.........ot closed successfully\n");
    }
    exit(1);
}
