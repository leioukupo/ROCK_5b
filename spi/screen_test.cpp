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
static const char *device = "/dev/spidev1.0"; // 操作的设备文件
// spi设置
static uint8_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay = 1;
int gpio[3] = {RES, DC, CS};
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
    char tx_data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // 定义读写的数据
    char rx_data[1] = {0};
    for (int j=0;j<3;j++) {
        unsigned char a=0xAE;
        OLED_WR_Byte(fp, spi_fp, a, reinterpret_cast<unsigned char *>(rx_data), 1, OLED_CMD);
        printf("read spi_rx_data is:\n"); // 读出总线的数据，引脚19与21短接打印【1,2,3,4,5,6,7,0,0,0】
        // 引脚19与21不短接打印【0,0,0,0,0,0,0,0,0,0】
        printf("%d\n", rx_data[0]);
        printf("\n");
        sleep(1);
    }
    Gpio_release();
//    u8 t;
//    OLED_Init(fp,fp_int); // 初始化OLED
//    OLED_Display_On(fp,fp_int);
}