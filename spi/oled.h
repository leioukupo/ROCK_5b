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
#ifndef SPI_OLED_H
#define SPI_OLED_H
#include <cstdio>
#define RES 100
#define DC 105
#define CS 114
#define  u8 unsigned char
#define  u32 unsigned int
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
#define OLED_MODE 0

void OLED_CS_Clr(FILE *fd);//  OLED_CS=0
void OLED_CS_Set(FILE *fd);//  OLED_CS=1

void OLED_RST_Clr(FILE *fd); //OLED_RST=0
void OLED_RST_Set(FILE *fd); //OLED_RST=1

void OLED_DC_Clr(FILE *fd);// OLED_DC=0
void OLED_DC_Set(FILE *fd);//OLED_DC=1



//OLED模式设置
//0:4线串行模式
//1:并行8080模式

#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF
#define X_WIDTH 	128
#define Y_WIDTH 	64
//-----------------OLED端口定义----------------

void ms_delay(unsigned int secs);
void Gpio_init(FILE **fd);
void Gpio_release();


//OLED控制用函数
void OLED_WR_Byte(FILE **fd,int fd_int, unsigned char tx_data, unsigned char *rx_data, int len, int  cmd);
void OLED_Display_On(FILE **fd,int fd_int);
void OLED_Display_Off(FILE **fd,int fd_int);
void OLED_Init(FILE **fd,int fd_int);
void OLED_Clear(FILE **fd,int fd_int);
//void OLED_DrawPoint(u8 x,u8 y,u8 t);
//void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(FILE **fd,int fd_int,u8 x, u8 y, u8 chr);
void OLED_ShowNum(FILE **fd,int fd_int,u8 x, u8 y, u32 num, u8 len, u8 size2);
void OLED_ShowString(FILE **fd,int fd_int,u8 x, u8 y, u8* chr);
void OLED_Set_Pos(FILE **fd,int fd_int,unsigned char x, unsigned char y);
void OLED_ShowCHinese(FILE **fd,int fd_int,u8 x, u8 y, u8 no);
void OLED_DrawBMP(FILE **fd,int fd_int,unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);
#endif //SPI_OLED_H
