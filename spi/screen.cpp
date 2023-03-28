//
// Created by leioukupo on 3/27/23.
//
#include <cstdint>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "oled.h"
//#define RES 100
//#define DC  105
//#define CS  114
static const char *device = "/dev/spidev1.0"; // 操作的设备文件
// spi设置
static uint8_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 500000;
//static uint16_t delay = 1;
//int gpio[3] = {RES, DC, CS};
FILE *fp[3] = {NULL, NULL, NULL};
int fp_int;
int main()
{
    int ret = 0;
    int spi_fp;
    spi_fp = open(device, O_RDWR);
    if (spi_fp < 0)
    {
        printf("设备打开失败\n");
    }
    // 设置mode
    ret = ioctl(spi_fp, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
    {
        printf("mode设置失败\n");
    }
    // 设置每个字节多少位
    ret = ioctl(spi_fp, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
    {
        printf("设置每个字节多少位失败\n");
    }
    // 无法获取每个字节的位数
    ret = ioctl(spi_fp, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
    {
        printf("无法获取每个字节的位数\n");
    }
    // 设置最大速率
    ret = ioctl(spi_fp, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    {
        printf("无法设置最大速率\n");
    }
    ret = ioctl(spi_fp, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    {
        printf("无法获取最大速率\n");
    }
    // 打印spi设置
    printf("spi 模式: %d\n", mode);
    printf("每个字的位数: %d\n", bits);
    printf("最大速率: %d Hz (%d KHz)\n", speed, speed / 1000);
    Gpio_init(fp);
    u8 t;
    OLED_Init(fp,spi_fp); // 初始化OLED
    t = ' ';
    for (int i=0;i<5;i++) {
        OLED_Clear(fp,spi_fp);
        OLED_ShowChar(fp,spi_fp,20,4,'h');
        OLED_ShowCHinese(fp,spi_fp,0, 0, 0); // 你
        OLED_ShowCHinese(fp,spi_fp,18, 0, 1); // 好
        OLED_ShowCHinese(fp,spi_fp,36, 0, 2); // 世
        OLED_ShowCHinese(fp,spi_fp,54, 0, 3); // 界
        OLED_ShowString(fp, spi_fp, 0, 2, (unsigned char *) "current temp");
        OLED_ShowString(fp,spi_fp,20, 4, (unsigned char *)"test");
        OLED_ShowString(fp,spi_fp,0, 6, (unsigned char *)"by");
        OLED_ShowString(fp,spi_fp,63, 6, (unsigned char *)"me");
//         OLED_ShowChar(48, 6, t); // 显示ASCII字符
//         t++;
//         if (t > '~')
//             t = ' ';
//         OLED_ShowNum(103, 6, t, 3, 16); // 显示ASCII字符的码值
        ms_delay(10000);
        OLED_Clear(fp,spi_fp);
        ms_delay(500);
    }
    Gpio_release();
    return 0;
}