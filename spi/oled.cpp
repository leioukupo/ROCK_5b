//
// Created by leioukupo on 3/27/23.
//
//              GND    电源地
//              VCC  接5V或3.3v电源
//              D0   GPIO3_C1	11（SCL）时钟
//              D1   GPIO3_B7	13（SDA）数据
//              RES  16	GPIO3_A4 100
//              DC   36	GPIO3_B1 105
//              CS   32	GPIO3_C2 114
#include "oled.h"
#include "gpio_common.h"
#include "font.h"
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

//int spi_int[3]={0,0,0,};
//FILE *fd_file;
//int goip[3] = {RES, DC, CS};
//FILE *fd[3] = {NULL, NULL, NULL};
//static const char *device = "/dev/spidev1.0"; // 操作的设备文件
//// spi设置
//static uint8_t mode = 0;
//static uint8_t bits = 8;
//static uint32_t speed = 500000;
//static uint16_t delay = 1;

void ms_delay(unsigned int secs) {
    struct timeval timeval;
    timeval.tv_sec = secs / 1000;
    timeval.tv_usec = (secs * 1000) % 1000000;
    select(0, NULL, NULL, NULL, &timeval);
}

void Gpio_init(FILE **fd) {
    char value_RES[64] = {0};
    char value_DC[64] = {0};
    char value_CS[64] = {0};
    gpio_init(RES, OUTPUT, NONE, value_RES, sizeof(value_RES));
    fd[0] = fopen(value_RES, "w");
    gpio_init(DC, OUTPUT, NONE, value_DC, sizeof(value_DC));
    fd[1] = fopen(value_DC, "w");
    gpio_init(CS, OUTPUT, NONE, value_CS, sizeof(value_CS));
    fd[2] = fopen(value_CS, "w");
}
void Gpio_release(){
    gpio_release(RES);
    gpio_release(DC);
    gpio_release(CS);
}
void OLED_CS_Clr(FILE *fd) {
    set_voltage(fd, 0);
}

void OLED_CS_Set(FILE *fd) {
    set_voltage(fd, 1);
}

void OLED_RST_Clr(FILE *fd) {
    set_voltage(fd, 0);
}

void OLED_RST_Set(FILE *fd) {
    set_voltage(fd, 1);
}

void OLED_DC_Clr(FILE *fd) {
    set_voltage(fd, 0);
}

void OLED_DC_Set(FILE *fd) {
    set_voltage(fd, 1);
}

void OLED_WR_Byte(FILE **fd, int fd_int, unsigned char tx_data, unsigned char *rx_data, int len, int cmd) {
    unsigned char *tx=&tx_data;
    unsigned char *rx={0};
    struct spi_ioc_transfer tr = {
            .tx_buf = (unsigned long) tx, // 发送缓存区
            .rx_buf = (unsigned long) rx_data, // 接收缓存区
            .len = 7,
            //  .speed_hz = 500000, // 总线速率
            .speed_hz = 500000,
            .delay_usecs = 1, // 发送时间间隔
            .bits_per_word = 8, // 收发的一个字的二进制位数
    };
    u8 i;
    if (cmd)
        OLED_DC_Set(fd[1]);
    else
        OLED_DC_Clr(fd[1]);
//    OLED_CS_Clr(fd[2]);
//发送数据
    int ret;
    ret = ioctl(fd_int, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        printf("can't send spi message\n");
//    OLED_CS_Set(fd[2]);
    OLED_DC_Set(fd[1]);
    //debug使用
//    printf("read spi_rx_data is:\n"); // 读出总线的数据
//    printf("%hhn\n", rx_data);
//    printf("\n");
}

void OLED_Set_Pos(FILE **fd, int fd_int, unsigned char x, unsigned char y) {
    unsigned long a = {0};
    OLED_WR_Byte(fd, fd_int, (0xb0 + y), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);
    OLED_WR_Byte(fd, fd_int, (((x & 0xf0) >> 4) | 0x10),
                 reinterpret_cast<unsigned char *>(a), 7, OLED_CMD);
    OLED_WR_Byte(fd, fd_int, ((x & 0x0f) | 0x01), reinterpret_cast<unsigned char *>(a),
                 7, OLED_CMD);
}

// 开启OLED显示
void OLED_Display_On(FILE **fd, int fd_int) {
    unsigned long a = {0};
    OLED_WR_Byte(fd, fd_int, (0X8D), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD); // SET DCDC命令
    OLED_WR_Byte(fd, fd_int, (0X14), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD); // DCDC ON
    OLED_WR_Byte(fd, fd_int, (0XAF), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD); // DISPLAY ON
}

// 关闭OLED显示
void OLED_Display_Off(FILE **fd, int fd_int) {
    unsigned long a = {0};
    OLED_WR_Byte(fd, fd_int, (0X8D), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD); // SET DCDC命令
    OLED_WR_Byte(fd, fd_int, (0X10), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD); // DCDC OFF
    OLED_WR_Byte(fd, fd_int, (0XAE), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);// DISPLAY OFF
}

// 清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void OLED_Clear(FILE **fd, int fd_int) {
    unsigned long a = {0};
    u8 i, n;
    for (i = 0; i < 8; i++) {
        OLED_WR_Byte(fd, fd_int, (0xb0 + i), reinterpret_cast<unsigned char *>(a), 7,
                     OLED_CMD);// 设置页地址（0~7）
        OLED_WR_Byte(fd, fd_int, (0x00), reinterpret_cast<unsigned char *>(a), 7,
                     OLED_CMD);// 设置显示位置—列低地址
        OLED_WR_Byte(fd, fd_int, (0x10), reinterpret_cast<unsigned char *>(a), 7,
                     OLED_CMD);// 设置显示位置—列高地址
        for (n = 0; n < 128; n++) {
            OLED_WR_Byte(fd, fd_int, (0), reinterpret_cast<unsigned char *>(a), 7,
                         OLED_DATA);
        }
    } // 更新显示
}

// 在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~63
// mode:0,反白显示;1,正常显示
// size:选择字体 16/12
void OLED_ShowChar(FILE **fd, int fd_int, u8 x, u8 y, u8 chr) {
    unsigned long a = {0};
    unsigned char c = 0, i = 0;
    c = chr - ' '; // 得到偏移后的值
    if (x > Max_Column - 1) {
        x = 0;
        y = y + 2;
    }
    if (SIZE == 16) {
        OLED_Set_Pos(fd, fd_int, x, y);
        for (i = 0; i < 8; i++) {
            OLED_WR_Byte(fd, fd_int, (F8X16[c * 16 + i]),
                         reinterpret_cast<unsigned char *>(a), 7, OLED_DATA);
        }
        OLED_Set_Pos(fd, fd_int, x, y + 1);
        for (i = 0; i < 8; i++) {
            OLED_WR_Byte(fd, fd_int, (F8X16[c * 16 + i + 8]),
                         reinterpret_cast<unsigned char *>(a), 7, OLED_DATA);
        }
    } else {
        OLED_Set_Pos(fd, fd_int, x, y + 1);
        for (i = 0; i < 6; i++) {
            OLED_WR_Byte(fd, fd_int, (F6x8[c][i]),
                         reinterpret_cast<unsigned char *>(a), 7, OLED_DATA);
        }
    }
}
// m^n函数
u32 oled_pow(u8 m, u8 n) {
    u32 result = 1;
    while (n--)
        result *= m;
    return result;
}

// 显示2个数字
// x,y :起点坐标
// len :数字的位数
// size:字体大小
// mode:模式	0,填充模式;1,叠加模式
// num:数值(0~4294967295);
void OLED_ShowNum(FILE **fd, int fd_int, u8 x, u8 y, u32 num, u8 len, u8 size2) {
    u8 t, temp;
    u8 enshow = 0;
    for (t = 0; t < len; t++) {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1)) {
            if (temp == 0) {
                OLED_ShowChar(fd, fd_int, (u8) (x + (size2 / 2) * t), (u8) y, ' ');
                continue;
            } else
                enshow = 1;
        }
        OLED_ShowChar(fd, fd_int, (u8) (x + (size2 / 2) * t), (u8) y, temp + '0');
    }
}

// 显示一个字符号串
void OLED_ShowString(FILE **fd, int fd_int, u8 x, u8 y, u8 *chr) {
    unsigned char j = 0;
    while (chr[j] != '\0') {
        OLED_ShowChar(fd, fd_int, (u8) x, (u8) y, chr[j]);
        x += 8;
        if (x > 120) {
            x = 0;
            y += 2;
        }
        j++;
    }
}

// 显示汉字
void OLED_ShowCHinese(FILE **fd, int fd_int, u8 x, u8 y, u8 no) {
    unsigned char a = {0};
    u8 t, adder = 0;
    OLED_Set_Pos(fd, fd_int, (u8) x, (u8) y);
    for (t = 0; t < 16; t++) {
        OLED_WR_Byte(fd, fd_int, (Hzk[2 * no][t]),
                     reinterpret_cast<unsigned char *>(a), 7, OLED_DATA);
        adder += 1;
    }
    OLED_Set_Pos(fd, fd_int, (u8) x, (u8) (y + 1));
    for (t = 0; t < 16; t++) {
        OLED_WR_Byte(fd, fd_int, (Hzk[2 * no + 1][t]),
                     reinterpret_cast<unsigned char *>(a), 7, OLED_DATA);
        adder += 1;
    }
}

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(FILE **fd, int fd_int, unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1,
                  unsigned char BMP[]) {
    unsigned int j = 0;
    unsigned char x, y;
    unsigned char a = {0};
    if (y1 % 8 == 0)
        y = y1 / 8;
    else
        y = y1 / 8 + 1;
    for (y = y0; y < y1; y++) {
        OLED_Set_Pos(fd, fd_int, (u8) x0, (u8) y);
        for (x = x0; x < x1; x++) {
            OLED_WR_Byte(fd, fd_int, (BMP[j++]), reinterpret_cast<unsigned char *>(a),
                         7, OLED_DATA);
        }
    }
}

// 初始化SSD1306
void OLED_Init(FILE **fd, int spi_int) {
    unsigned char a = {0};
    OLED_RST_Set(fd[0]);
    ms_delay(100);
    OLED_RST_Clr(fd[0]);
    ms_delay(100);
    OLED_RST_Set(fd[0]);
    /*
    OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
    OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
    OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
    OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
    OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
    OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
    OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
    OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_Byte(0x00,OLED_CMD);//-not offset
    OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
    OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
    OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
    OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WR_Byte(0x02,OLED_CMD);//
    OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
    OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
    OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7)
    OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
    */
    OLED_WR_Byte(fd, spi_int, (0xAE), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--turn off oled panel
    OLED_WR_Byte(fd, spi_int, (0x00), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//---set low column address
    OLED_WR_Byte(fd, spi_int, (0x10), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//---set high column address
    OLED_WR_Byte(fd, spi_int, (0x40), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WR_Byte(fd, spi_int, (0x81), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--set contrast control register
    OLED_WR_Byte(fd, spi_int, (0xCF), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);// Set SEG Output Current Brightness
    OLED_WR_Byte(fd, spi_int, (0xA1), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WR_Byte(fd, spi_int, (0xC8), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);// Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WR_Byte(fd, spi_int, (0xA6), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--set normal display
    OLED_WR_Byte(fd, spi_int, (0xA8), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(fd, spi_int, (0x3f), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--1/64 duty
    OLED_WR_Byte(fd, spi_int, (0xD3), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_Byte(fd, spi_int, (0x00), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//-not offset
    OLED_WR_Byte(fd, spi_int, (0xd5), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(fd, spi_int, (0x80), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_Byte(fd, spi_int, (0xD9), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--set pre-charge period
    OLED_WR_Byte(fd, spi_int, (0xF1), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);// Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_Byte(fd, spi_int, (0xDA), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--set com pins hardware configuration
    OLED_WR_Byte(fd, spi_int, (0x12), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);
    OLED_WR_Byte(fd, spi_int, (0xDB), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--set vcomh
    OLED_WR_Byte(fd, spi_int, (0x40), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);// Set VCOM Deselect Level
    OLED_WR_Byte(fd, spi_int, (0x20), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WR_Byte(fd, spi_int, (0x02), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//
    OLED_WR_Byte(fd, spi_int, (0x8D), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--set Charge Pump enable/disable
    OLED_WR_Byte(fd, spi_int, (0x14), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--set(0x10) disable
    OLED_WR_Byte(fd, spi_int, (0xA4), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(fd, spi_int, (0xA6), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);// Disable Inverse Display On (0xa6/a7)
    OLED_WR_Byte(fd, spi_int, (0xAF), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);//--turn on oled panel
    OLED_WR_Byte(fd, spi_int, (0xAF), reinterpret_cast<unsigned char *>(a), 7,
                 OLED_CMD);/*display ON*/
    OLED_Clear(fd, spi_int);
    OLED_Set_Pos(fd, spi_int, (u8) 0, (u8) 0);
}

