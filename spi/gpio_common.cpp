//
// Created by leioukupo on 3/15/23.
//
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "gpio_common.h"

int gpio_init(int gpio,DIRECTION direction,EDGE edge,char* value,int value_len)
{
    char cmd[1024] = {0};
    char file[64] = {0};
    FILE * fd_tmp;

    memset(cmd,0,sizeof(cmd));
    sprintf(cmd,"echo %d > /sys/class/gpio/export",gpio);
    system(cmd);

    if ( direction == INPUT )
    {
        memset(file,0,sizeof(file));
        sprintf(file,"/sys/class/gpio/gpio%d/direction",gpio);
        fd_tmp = fopen(file,"w");
        if (fd_tmp == NULL)
        {
            printf("open %s fail\n",file);
            return -1;
        }
        fprintf(fd_tmp,"in");
        fclose(fd_tmp);
    }
    else if ( direction == OUTPUT )
    {
        memset(file,0,sizeof(file));
        sprintf(file,"/sys/class/gpio/gpio%d/direction",gpio);
        fd_tmp = fopen(file,"w");
        if (fd_tmp == NULL)
        {
            printf("open %s fail\n",file);
            return -1;
        }
        fprintf(fd_tmp,"out");
        fclose(fd_tmp);
    }
    if ( edge == NONE )
    {
        memset(file,0,sizeof(file));
        sprintf(file,"/sys/class/gpio/gpio%d/edge",gpio);
        fd_tmp = fopen(file,"w");
        if (fd_tmp == NULL)
        {
            printf("open %s fail\n",file);
            return -1;
        }

        fprintf(fd_tmp,"none");
        fclose(fd_tmp);
    }
    else if ( edge == RISING )
    {
        memset(file,0,sizeof(file));
        sprintf(file,"/sys/class/gpio/gpio%d/edge",gpio);
        fd_tmp = fopen(file,"w");
        if (fd_tmp == NULL)
        {
            printf("open %s fail\n",file);
            return -1;
        }

        fprintf(fd_tmp,"rising");
        fclose(fd_tmp);
    }
    else if ( edge == FALLING )
    {
        memset(file,0,sizeof(file));
        sprintf(file,"/sys/class/gpio/gpio%d/edge",gpio);
        fd_tmp = fopen(file,"w");
        if (fd_tmp == NULL)
        {
            printf("open %s fail\n",file);
            return -1;
        }

        fprintf(fd_tmp,"falling");
        fclose(fd_tmp);
    }
    else if ( edge == BOTH )
    {
        memset(file,0,sizeof(file));
        sprintf(file,"/sys/class/gpio/gpio%d/edge",gpio);
        fd_tmp = fopen(file,"w");
        if (fd_tmp == NULL)
        {
            printf("open %s fail\n",file);
            return -1;
        }

        fprintf(fd_tmp,"both");
        fclose(fd_tmp);
    }
    if (value != NULL && value_len > 0)
    {
        memset(value,0,value_len);
        snprintf(value,value_len,"/sys/class/gpio/gpio%d/value",gpio);
    }
    return 0;
}
int set_voltage(FILE * fd,int value)
{
    if (fd != NULL)
    {
        if (value == 0 )
        {
            fprintf(fd,"0");
        }
        else
        {
            fprintf(fd,"1");
        }
        rewind(fd);
        return 0;
    }
    else
    {
        return -1;
    }
}
int get_voltage(int fd)
{
    char buffer[16];
    int len;

    if((len=read(fd,buffer,sizeof(buffer)))==-1)
    {
        perror("read failed!\n");
        return -1;
    }
    buffer[len]=0;
    return atoi(buffer);
}
void gpio_release(int gpio)
{
    char cmd[1024];
    memset(cmd,0,sizeof(cmd));
    sprintf(cmd,"echo %d > /sys/class/gpio/unexport",gpio);
    system(cmd);
}
