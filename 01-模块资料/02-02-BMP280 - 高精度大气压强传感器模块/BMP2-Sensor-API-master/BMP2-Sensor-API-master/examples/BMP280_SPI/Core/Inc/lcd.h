#ifndef __LCD_H
#define __LCD_H        

#include "main.h"

/***********************************************
*LCD重要参数集
************************************************/
typedef struct  
{                                            
    uint16_t width;      //LCD 宽度
    uint16_t height;     //LCD 高度
    uint16_t id;         //LCD ID
    uint8_t  dir;        //横屏还是竖屏控制：0，竖屏；1，横屏。    
    uint16_t wramcmd;    //开始写gram指令
    uint16_t setxcmd;    //设置x坐标指令
    uint16_t setycmd;    //设置y坐标指令 
}LCD_Dev_Structure;	  

/***********************************************
*LCD地址结构体
************************************************/
typedef struct
{
	volatile uint16_t LCD_REG;
	volatile uint16_t LCD_RAM;
}LCD_TypeDef;

/***********************************************
*LCD端口定义
*使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11 A10作为数据命令区分线 
*注意设置时STM32内部会右移一位对其!
************************************************/ 			    
#define LCD_BASE        ((uint32_t)(0x6C000000 | 0x000007FE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

/***********************************************
*LCD参数
************************************************/
extern LCD_Dev_Structure lcddev;	//管理LCD重要参数

/***********************************************
*LCD的画笔颜色和背景色	
************************************************/
extern uint16_t  POINT_COLOR;//默认红色    
extern uint16_t  BACK_COLOR; //背景颜色.默认为白色

/***********************************************
*LCD背光
************************************************/
#define LCD_BL_SET()   HAL_GPIO_WritePin(LCD_BL_GPIO_Port,LCD_BL_Pin,GPIO_PIN_SET)
#define LCD_BL_RESET() HAL_GPIO_WritePin(LCD_BL_GPIO_Port,LCD_BL_Pin,GPIO_PIN_RESET)

/***********************************************
*扫描方向定义
************************************************/
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上
#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左     

#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向

/***********************************************
*画笔颜色(GUI颜色)
************************************************/
#define WHITE   0xFFFF
#define BLACK   0x0000      
#define BLUE    0x001F  
#define BRED    0XF81F
#define GRED    0XFFE0
#define GBLUE   0X07FF
#define RED     0xF800
#define MAGENTA 0xF81F
#define GREEN   0x07E0
#define CYAN    0x7FFF
#define YELLOW  0xFFE0
#define BROWN   0XBC40 //棕色
#define BRRED   0XFC07 //棕红色
#define GRAY    0X8430 //灰色

/***********************************************
*以下三色为PANEL的颜色 
************************************************/
#define DARKBLUE    0X01CF  //深蓝色
#define LIGHTBLUE   0X7D7C  //浅蓝色  
#define GRAYBLUE    0X5458  //灰蓝色
 
#define LIGHTGREEN  0X841F //浅绿色
#define LIGHTGRAY   0XEF5B //浅灰色(PANNEL)
#define LGRAY       0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE   0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE      0X2B12 //浅棕蓝色(选择条目的反色)

/***********************************************
*LCD分辨率设置
************************************************/
#define SSD_HOR_RESOLUTION      800 //LCD水平分辨率
#define SSD_VER_RESOLUTION      480 //LCD垂直分辨率

/***********************************************
*LCD驱动参数设置
************************************************/
#define SSD_HOR_PULSE_WIDTH     1   //水平脉宽
#define SSD_HOR_BACK_PORCH      210 //水平前廊
#define SSD_HOR_FRONT_PORCH     45  //水平后廊

#define SSD_VER_PULSE_WIDTH     1   //垂直脉宽
#define SSD_VER_BACK_PORCH      34  //垂直前廊
#define SSD_VER_FRONT_PORCH     10  //垂直前廊

/***********************************************
*如下几个参数，自动计算
************************************************/
#define SSD_HT  (SSD_HOR_RESOLUTION+SSD_HOR_PULSE_WIDTH+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS (SSD_HOR_PULSE_WIDTH+SSD_HOR_BACK_PORCH)
#define SSD_VT  (SSD_VER_PULSE_WIDTH+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH+SSD_VER_RESOLUTION)
#define SSD_VSP (SSD_VER_PULSE_WIDTH+SSD_VER_BACK_PORCH)

/***********************************************
 *初始化lcd
 *函数名：void LCD_Init(void)
 *参数：无
 *返回值：无 
 *时间：2018-08-29
 *备注：该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
        在其他型号的驱动芯片上没有测试! 
************************************************/
extern void LCD_Init(void);

/***********************************************
 *LCD开启显示
 *函数名：void LCD_DisplayOn(void)
 *参数：无
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_DisplayOn(void);

/***********************************************
 *LCD关闭显示
 *函数名：void LCD_DisplayOff(void)
 *参数：无
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_DisplayOff(void);

/***********************************************
 *清屏函数
 *函数名：void LCD_Clear(uint16_t color)
 *参数：color-要清屏的填充色
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_Clear(uint16_t Color);

/***********************************************
 *设置光标位置
 *函数名：void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
 *参数：Xpos:横坐标;Ypos:纵坐标
 *返回值：无
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);

/***********************************************
 *画点
 *函数名：void LCD_DrawPoint(uint16_t x,uint16_t y)
 *参数：x,y-坐标
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_DrawPoint(uint16_t x,uint16_t y);

/***********************************************
 *快速画点
 *函数名：void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
 *参数：x,y-坐标;color-颜色
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color);

/***********************************************
 *读取个某点的颜色值
 *函数名：u16 LCD_ReadPoint(uint16_t x,uint16_t y)
 *参数：x,y-坐标
 *返回值：此点的颜色 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern uint16_t  LCD_ReadPoint(uint16_t x,uint16_t y);

/***********************************************
 *在指定位置画一个指定大小的圆
 *函数名：void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r)
 *参数：(x,y)-中心点
        r-半径
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r);

/***********************************************
 *画线
 *函数名：void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
 *参数：x1,y1-起点坐标
        x2,y2-终点坐标  
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/***********************************************
 *画矩形
 *函数名：void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
 *参数：(x1,y1),(x2,y2)-矩形的对角坐标
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/***********************************************
 *在指定区域内填充单个颜色
 *函数名：void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
 *参数：(sx,sy),(ex,ey)-填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
        color-要填充的颜色
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color); 

/***********************************************
 *在指定区域内填充指定颜色块
 *函数名：void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
 *参数：(sx,sy),(ex,ey)-填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
        color-要填充的颜色
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color);

/***********************************************
 *在指定位置显示一个字符
 *函数名：void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
 *参数：x,y-起始坐标
        num-要显示的字符:" "--->"~"
        size-字体大小 12/16/24
        mode-叠加方式(1)还是非叠加方式(0)
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode);

/***********************************************
 *显示数字,高位为0,则不显示
 *函数名：void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size)
 *参数：x,y -起点坐标	 
        len -数字的位数
        size-字体大小
        color-颜色 
        num-数值(0~4294967295);
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/	
extern void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size);

/***********************************************
 *显示数字,高位为0,还是显示
 *函数名：void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode)
 *参数：x,y-起点坐标
        num-数值(0~999999999);	 
        len-长度(即要显示的位数)
        size-字体大小
        mode:
            [7]:0,不填充;1,填充0.
            [6:1]:保留
            [0]:0,非叠加显示;1,叠加显示.
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/	
extern void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode);

/***********************************************
 *显示字符串
 *函数名：void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p)
 *参数：x,y-起点坐标
        width,height-区域大小  
        size-字体大小
        *p-字符串起始地址	
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/	
extern void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p);

/***********************************************
 *LCD写寄存器及数据函数
 *函数名：void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
 *参数：LCD_Reg-寄存器地址
        LCD_RegValue-要写入的数据
 *返回值：读到的值 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue);

/***********************************************
 *LCD读寄存器
 *函数名：u16 LCD_ReadReg(uint16_t LCD_Reg)
 *参数：LCD_Reg-寄存器地址
 *返回值：读到的数据 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern uint16_t LCD_ReadReg(uint16_t LCD_Reg);

/***********************************************
 *LCD开始写GRAM
 *函数名：void LCD_WriteRAM_Prepare(void)
 *参数：无
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_WriteRAM_Prepare(void);

/***********************************************
 *LCD写GRAM
 *函数名：void LCD_WriteRAM(uint16_t RGB_Code)
 *参数：RGB_Code-颜色值
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_WriteRAM(uint16_t RGB_Code);

/***********************************************
 *SSD1963 背光设置
 *函数名：void LCD_SSD_BackLightSet(uint8_t pwm)
 *参数：pwm-背光等级,0~100.越大越亮.
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_SSD_BackLightSet(uint8_t pwm);

/***********************************************
 *设置LCD的自动扫描方向
 *函数名：void LCD_Scan_Dir(uint8_t dir)
 *参数：dir:0~7,代表8个方向(具体定义见lcd.h)
 *返回值：无 
 *时间：2018-08-29
 *备注：9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963等IC已经实际测试
        其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
************************************************/	
extern void LCD_Scan_Dir(uint8_t dir);

/***********************************************
 *设置LCD显示方向
 *函数名：void LCD_Display_Dir(uint8_t dir)
 *参数：dir-0,竖屏；1,横屏
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_Display_Dir(uint8_t dir);

/***********************************************
 *设置窗口,并自动设置画点坐标到窗口左上角(sx,sy)
 *函数名：void LCD_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
 *参数：sx,sy-窗口起始坐标(左上角)
        width*height-窗体大小
 *返回值：无 
 *时间：2018-08-29
 *备注：基于正点原子STM32F1精英版
************************************************/
extern void LCD_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);  

#endif  
     
     



