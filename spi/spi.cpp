//
// Created by leioukupo on 3/15/23.
//
#include <cstdint>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "font.h"
#include "spi.h"
#include "gpio_common.h"

void OLED_RST_Set(int gpio_number){
    char value[64] = {0};
    FILE *fd = nullptr;
    gpio_init(gpio_number, OUTPUT, NONE, value, sizeof(value));
    fd = fopen(value, "w");
    set_voltage(fd, 1);
    gpio_release(gpio_number);
}
void OLED_RST_Clr(int gpio_number)
{
    char value[64] = {0};
    FILE *fd = NULL;
    gpio_init(gpio_number, OUTPUT, NONE, value, sizeof(value));
    fd = fopen(value, "w");
    set_voltage(fd, 0);
    gpio_release(gpio_number);
}
void OLED_WR_Byte(int fd, u8 dat)
{
    uint8_t tx[] = {
        0xb0};
    uint8_t rx[ARRAY_SIZE(tx)] = {
        0,
    };
    struct spi_ioc_transfer tr = {

        .tx_buf = (unsigned long)tx, // 发送缓存区

        .rx_buf = (unsigned long)rx, // 接收缓存区

        .len = ARRAY_SIZE(tx),

        .speed_hz = 500000, // 总线速率

        .delay_usecs = 1,   // 发送时间间隔

        .bits_per_word = 8, // 收发的一个字的二进制位数
    };
    int ret;
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        printf("can't send spi message\n");
}
void OLED_Set_Pos(int fd, unsigned char x, unsigned char y)
{
    OLED_WR_Byte(fd, 0xb0 + y);
    OLED_WR_Byte(fd, ((x & 0xf0) >> 4) | 0x10);
    OLED_WR_Byte(fd, (x & 0x0f) | 0x01);
}
// 开启OLED显示
void OLED_Display_On(int fd)
{
    OLED_WR_Byte(fd, 0X8D); // SET DCDC命令
    OLED_WR_Byte(fd, 0X14); // DCDC ON
    OLED_WR_Byte(fd, 0XAF); // DISPLAY ON
}
// 关闭OLED显示
void OLED_Display_Off(int fd)
{
    OLED_WR_Byte(fd, 0X8D); // SET DCDC命令
    OLED_WR_Byte(fd, 0X10); // DCDC OFF
    OLED_WR_Byte(fd, 0XAE); // DISPLAY OFF
}
// 清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void OLED_Clear(int fd)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WR_Byte(fd, 0xb0 + i); // 设置页地址（0~7）
        OLED_WR_Byte(fd, 0x00);     // 设置显示位置—列低地址
        OLED_WR_Byte(fd, 0x10);     // 设置显示位置—列高地址
        for (n = 0; n < 128; n++)
            OLED_WR_Byte(fd, 0);
    } // 更新显示
}
// 在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~63
// mode:0,反白显示;1,正常显示
// size:选择字体 16/12
static void OLED_ShowChar(int fd, u8 x, u8 y, u8 chr)
{
    unsigned char c = 0, i = 0;
    c = chr - ' '; // 得到偏移后的值
    if (x > Max_Column - 1)
    {
        x = 0;
        y = y + 2;
    }
    if (SIZE == 16)
    {
        OLED_Set_Pos(fd, x, y);
        for (i = 0; i < 8; i++)
            OLED_WR_Byte(fd, F8X16[c * 16 + i]);
        OLED_Set_Pos(fd, x, y + 1);
        for (i = 0; i < 8; i++)
            OLED_WR_Byte(fd, F8X16[c * 16 + i + 8]);
    }
    else
    {
        OLED_Set_Pos(fd, x, y + 1);
        for (i = 0; i < 6; i++)
            OLED_WR_Byte(fd, F6x8[c][i]);
    }
}
// m^n函数
unsigned int oled_pow(u8 m, u8 n)
{
    unsigned int result = 1;
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

void OLED_ShowNum(int fd, u8 x, u8 y, u32 num, u8 len, u8 size2)
{
    u8 t, temp;
    u8 enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(fd, x + (size2 / 2) * t, y, ' ');
                continue;
            }
            else
                enshow = 1;
        }
        OLED_ShowChar(fd, x + (size2 / 2) * t, y, temp + '0');
    }
}
// 显示一个字符号串
void OLED_ShowString(int fd, u8 x, u8 y, unsigned char *chr)
{
    unsigned char j = 0;
    while (chr[j] != '\0')
    {
        OLED_ShowChar(fd, x, y, chr[j]);
        x += 8;
        if (x > 120)
        {
            x = 0;
            y += 2;
        }
        j++;
    }
}
// 显示汉字
void OLED_ShowCHinese(int fd, u8 x, u8 y, u8 no)
{
    u8 t, adder = 0;
    OLED_Set_Pos(fd, x, y);
    for (t = 0; t < 16; t++)
    {
        OLED_WR_Byte(fd, Hzk[2 * no][t]);
        adder += 1;
    }
    OLED_Set_Pos(fd, x, y + 1);
    for (t = 0; t < 16; t++)
    {
        OLED_WR_Byte(fd, Hzk[2 * no + 1][t]);
        adder += 1;
    }
}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(int fd, unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
    unsigned int j = 0;
    unsigned char x, y;

    if (y1 % 8 == 0)
        y = y1 / 8;
    else
        y = y1 / 8 + 1;
    for (y = y0; y < y1; y++)
    {
        OLED_Set_Pos(fd, x0, y);
        for (x = x0; x < x1; x++)
        {
            OLED_WR_Byte(fd, BMP[j++]);
        }
    }
}
// 初始化SSD1306
void OLED_Init(int fd)
{

    OLED_RST_Set(100);
    sleep(1);
    // delay_ms(100);
    OLED_RST_Clr(100);
    sleep(1);
    // delay_ms(100);
    OLED_RST_Set(100);
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

    OLED_WR_Byte(fd, 0xAE); //--turn off oled panel
    OLED_WR_Byte(fd, 0x00); //---set low column address
    OLED_WR_Byte(fd, 0x10); //---set high column address
    OLED_WR_Byte(fd, 0x40); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WR_Byte(fd, 0x81); //--set contrast control register
    OLED_WR_Byte(fd, 0xCF); // Set SEG Output Current Brightness
    OLED_WR_Byte(fd, 0xA1); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WR_Byte(fd, 0xC8); // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WR_Byte(fd, 0xA6); //--set normal display
    OLED_WR_Byte(fd, 0xA8); //--set multiplex ratio(1 to 64)
    OLED_WR_Byte(fd, 0x3f); //--1/64 duty
    OLED_WR_Byte(fd, 0xD3); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_Byte(fd, 0x00); //-not offset
    OLED_WR_Byte(fd, 0xd5); //--set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(fd, 0x80); //--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_Byte(fd, 0xD9); //--set pre-charge period
    OLED_WR_Byte(fd, 0xF1); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_Byte(fd, 0xDA); //--set com pins hardware configuration
    OLED_WR_Byte(fd, 0x12);
    OLED_WR_Byte(fd, 0xDB); //--set vcomh
    OLED_WR_Byte(fd, 0x40); // Set VCOM Deselect Level
    OLED_WR_Byte(fd, 0x20); //-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WR_Byte(fd, 0x02); //
    OLED_WR_Byte(fd, 0x8D); //--set Charge Pump enable/disable
    OLED_WR_Byte(fd, 0x14); //--set(0x10) disable
    OLED_WR_Byte(fd, 0xA4); // Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(fd, 0xA6); // Disable Inverse Display On (0xa6/a7)
    OLED_WR_Byte(fd, 0xAF); //--turn on oled panel
    OLED_WR_Byte(fd, 0xAF); /*display ON*/
    OLED_Clear(fd);
    OLED_Set_Pos(fd, 0, 0);
}
