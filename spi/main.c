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

static const char *device = "/dev/spidev1.0"; // 操作的设备文件
// spi设置
static uint8_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay = 1;

int OLED_WR_Byte(int fd, unsigned char *tx_data, unsigned char *rx_data, int len)
{
    struct spi_ioc_transfer tr = {

            .tx_buf = (unsigned long)tx_data, // 发送缓存区

            .rx_buf = (unsigned long)rx_data, // 接收缓存区

            .len = len,
//            .speed_hz = 500000, // 总线速率
            .speed_hz = 500000,
            .delay_usecs = 0, // 发送时间间隔

            .bits_per_word = 8, // 收发的一个字的二进制位数
    };
     int ret;
     ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
     if (ret < 1)
         printf("can't send spi message\n");
}
int main(void)
{
    char tx_Data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // 定义读写的数据
    char rx_Data[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
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
    for (int j=0;j<3;j++)
    {
        OLED_WR_Byte(fd, tx_Data, rx_Data, 7);
        printf("read spi_rx_data is:\n"); // 读出总线的数据，引脚19与21短接打印【1,2,3,4,5,6,7,0,0,0】

        // 引脚19与21不短接打印【0,0,0,0,0,0,0,0,0,0】

        for (int i = 0; i < 10; i++)

        {
            printf("%d\n", rx_Data[i]);
        }

        printf("\n");
        sleep(5);

    }
    close(fd);
    return 0;
}