//
// Created by leioukupo on 3/15/23.
//
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "spi.h"

static const char *device = "/dev/spidev0.0"; // 操作的设备文件
// spi设置
static uint8_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay = 1;

int main(void){
    int ret = 0;
    int fd;
    fd = open(device, O_RDWR);
    if (fd < 0)
    {
        printf("设备打开失败\n");
    }
    // 设置mode
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
    {
        printf("mode设置失败\n");
    }
    // 设置每个字节多少位
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
    {
        printf("设置每个字节多少位失败\n");
    }
    // 无法获取每个字节的位数
    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
    {
        printf("无法获取每个字节的位数\n");
    }
    // 设置最大速率
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    {
        printf("无法设置最大速率\n");
    }
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    {
        printf("无法获取最大速率\n");
    }
    // 打印spi设置
    printf("spi 模式: %d\n", mode);
    printf("每个字的位数: %d\n", bits);
    printf("最大速率: %d Hz (%d KHz)\n", speed, speed / 1000);
    u8 t;
    OLED_Init(fd); // 初始化OLED
    OLED_Display_On(fd);
    printf("初始化\n");
    t = ' ';
    unsigned char a[] = "current temp";
    unsigned char b[] = "test";
    unsigned char c[] = "by";
    unsigned char d[] = "me";
    while (1)
    {
        OLED_Clear(fd);
        OLED_ShowCHinese(fd, 0, 0, 0);  // 你
        OLED_ShowCHinese(fd, 18, 0, 1); // 好
        OLED_ShowCHinese(fd, 36, 0, 2); // 世
        OLED_ShowCHinese(fd, 54, 0, 3); // 界
        printf("显示你好世界\n");
//        OLED_ShowString(fd, 0, 2, a);
//        OLED_ShowString(fd, 20, 4, b);
//        OLED_ShowString(fd, 0, 6, c);
//        OLED_ShowString(fd, 63, 6, d);
        // OLED_ShowChar(48, 6, t); // 显示ASCII字符
        // t++;
        // if (t > '~')
        //     t = ' ';
        // OLED_ShowNum(103, 6, t, 3, 16); // 显示ASCII字符的码值
        sleep(3);
//        OLED_Clear(fd);
//        sleep(1);
    }
    return 0;
}