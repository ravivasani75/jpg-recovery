#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void recover_jpeg(FILE* raw_file);

int main(int argc, char *argv[])
{

    if(argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }

    FILE* raw_file;
    if( (raw_file = fopen(argv[1], "rb")) == NULL)
    {
        printf("Couldn't open %s\n", argv[1]);
        return 1;
    }

    recover_jpeg(raw_file);

    fclose(raw_file);
    return 0;
}

void recover_jpeg(FILE* raw_file)
{
    FILE* image_file = NULL;
    unsigned char buf[512];
    int counter = 0;

    char* image_name = malloc(128);
    if(!image_name)
    {
        printf("Memory resource error, try again\n");
        return;
    }
    int readed;
    int got_FF = 0; // whether 0xff was last byte of previous block or not
    while((readed = fread(buf, 1, 512, raw_file)))
    {

        if(!image_file && buf[0] == 0xff && buf[1] == 0xd8 && buf[2] == 0xff && buf[3] >= 0xe0 && buf[3] <= 0xef )
        {
            got_FF = 0;
            counter++;
            sprintf(image_name, "%03d.jpg", counter);
            if((image_file = fopen(image_name, "wb")) == NULL)
            {
                free(image_name);
                printf("Memory resource error, try again!\n");
                return;
            }
        }

        if(image_file)
        {
            int size = 1;
            if(got_FF && buf[0] == 0xd9)
            {
                printf("got_FF && buf[0] == 0xd9\n");
                fwrite(buf, 1, size, image_file);
                fclose(image_file);
                image_file = NULL;
            }
            else
                for(size = 2; size <= readed; ++size)
                {
                    if(buf[size-2] == 0xff && buf[size-1] == 0xd9)
                    {
                        fwrite(buf, 1, size, image_file);
                        fclose(image_file);
                        image_file = NULL;
                        break;
                    }
                }

            if(image_file)
            {
                fwrite(buf, 1, readed, image_file);
            }

            if(image_file && buf[readed-1] == 0xff)
            {
                got_FF = 1;
            }
            else
            {
                got_FF = 0;
            }
        }

    }
    if(image_file)
        free(image_file);

    free(image_name);

}