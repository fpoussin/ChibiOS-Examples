 
#define LCD_USE_FSMC

//#define LCD_USE_GPIO
#if defined(LCD_USE_GPIO)

    #define LCD_CMD_PORT GPIOC
    #define LCD_DATA_PORT GPIOD

    #define LCD_CS 0
    #define LCD_RS 1
    #define LCD_WR 2
    #define LCD_RD 3
#endif