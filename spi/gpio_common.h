//
// Created by leioukupo on 3/15/23.
//

#ifndef SPI_GPIO_COMMON_H
#define SPI_GPIO_COMMON_H
#ifndef _GPIO_H_
#define _GPIO_H_
#include <stdio.h>

typedef enum
{
    INVALI_DIRECTION [[maybe_unused]],
    INPUT,
    OUTPUT,
} DIRECTION;

typedef enum
{
    INVALI_EDGE [[maybe_unused]],
    NONE,
    RISING,
    FALLING,
    BOTH,
} EDGE;

int gpio_init(int gpio,DIRECTION direction,EDGE edge,char* value,int value_len);
int set_voltage(FILE * fd,int value);

int get_voltage(int fd);
void gpio_release(int gpio);
#endif

#endif //SPI_GPIO_COMMON_H
