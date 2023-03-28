//
// Created by leioukupo on 3/15/23.
//

#ifndef SPI_SPI_H
#define SPI_SPI_H
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define u8 unsigned char
#define u32 unsigned int
#define SIZE 16
#define XLevelL 0x02
#define XLevelH 0x10
#define Max_Column 128
#define Max_Row 64
#define Brightness 0xFF
#define X_WIDTH 128
#define Y_WIDTH 64
void OLED_RST_Set(int gpio_number);
void OLED_RST_Clr(int gpio_number);
void OLED_WR_Byte(int fd, u8 dat);
void OLED_Display_On(int fd);
void OLED_Display_Off(int fd);
void OLED_Clear(int fd);
static void OLED_ShowChar(int fd, u8 x, u8 y, u8 chr);
unsigned int oled_pow(u8 m, u8 n);
void OLED_ShowNum(int fd, u8 x, u8 y, u32 num, u8 len, u8 size2);
void OLED_ShowString(int fd, u8 x, u8 y, unsigned char *chr);
void OLED_ShowCHinese(int fd, u8 x, u8 y, u8 no);
void OLED_DrawBMP(int fd, unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);
void OLED_Init(int fd);
#endif // SPI_SPI_H
