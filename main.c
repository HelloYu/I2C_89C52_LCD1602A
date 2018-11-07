#include <reg52.h>
#include <intrins.h>

#include "LCD1602_I2C.h"

sbit SCL = P1 ^ 0;
sbit SDA = P1 ^ 1;

uchar PCF8574_ADDR = 0x4E; // write address

uchar display_control;
uchar display_function;
uchar display_mode;
uchar * text = "I AM PIG";

void delay_4us(void);
void delay_ms(uint);

// RS H:Data Input L:Instruction Input
// RW H--Read L--Write
void main()
{
    init_I2C();
    init_LCD1602();
    delay_ms(1000);

    print_str(text);

    while(1);
}

void init_LCD1602()
{
    display_function = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;

    delay_ms(50);
    // we start in 8bit mode, try to set 4 bit mode
    I2C_write_expender(0x03 << 4);
    delay_ms(5);
    I2C_write_expender(0x03 << 4);
    delay_ms(5);
    I2C_write_expender(0x03 << 4);
    delay_ms(5);

    // finally, set to 4-bit interface
    I2C_write_expender(0x02 << 4);

    // set # lines, font size, etc.
    I2C_write_com(LCD_FUNCTIONSET | display_function);

    // turn the display on with no cursor or blinking default
    display_control = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();

    // clear it off
    clear();

    // Initialize to default text direction (for roman languages)
    display_mode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

    // set the entry mode
    I2C_write_com(LCD_ENTRYMODESET | display_mode);

    home();
}

void print_str(uchar * dat) {
    uchar i;

    I2C_write_com(LCD_SETDDRAMADDR);
    i = 0;
    while (dat[i] != '\0') {
        I2C_write_data(dat[i]);
        i++;
    }
}
void clear()
{
    I2C_write_com(LCD_CLEARDISPLAY);
    delay_ms(2000);
}

void home()
{
    I2C_write_com(LCD_RETURNHOME);
    delay_ms(2000);
}

void display()
{
    display_control |= LCD_DISPLAYON;
    I2C_write_com(LCD_DISPLAYCONTROL |display_control);
}

void init_I2C()
{
    SDA = I2C_HIGH;
    delay_4us();
    SCL = I2C_HIGH;
    delay_4us();
}

void I2C_write_byte(uchar dat)
{
    uchar i, tmp;
    tmp = dat;
    for (i = 0; i < 8; i++)
    {
        tmp = tmp << 1;
        SCL = I2C_LOW;
        delay_4us();
        SDA = CY;
        delay_4us();
        SCL = I2C_HIGH;
        delay_4us();
    }
    SCL = I2C_LOW;
    delay_4us();
    SDA = I2C_HIGH;
    delay_4us();
}

void I2C_send(uchar dat, uchar mode)
{
    uchar MSB = dat & 0xF0;

    uchar LSB = (dat << 4) & 0xF0;

    I2C_write_expender((MSB) | mode);
    I2C_write_expender((LSB) | mode);
    delay_ms(50);
}

void I2C_write_expender(uchar dat)
{
    uchar tmp;

    I2C_start();

    I2C_write_byte(PCF8574_ADDR);
    I2C_ACK();
    delay_4us();

    tmp = dat;
    tmp |= 0x0C; //RS = 0, RW = 0, EN = 1, p3=1

    I2C_write_byte(tmp);
    I2C_ACK();
    delay_ms(1);

    tmp &= 0xFB; //Make EN = 0
    I2C_write_byte(tmp);
    I2C_ACK();
    delay_ms(1);

    I2C_stop();
}

void I2C_write_data(uchar dat)
{
    I2C_send(dat, 1);
}

void I2C_write_com(uchar dat)
{
    I2C_send(dat, 0);
}

/**
 * @brief Response check 
 * 
 */
void I2C_ACK()
{
    uchar i;
    SCL = I2C_HIGH;
    delay_4us();
    while ((SDA == I2C_HIGH) && (i < 250))
        i++;
    SCL = I2C_LOW;
    delay_4us();
}

void I2C_start()
{
    SDA = I2C_HIGH;
    delay_4us();
    SCL = I2C_HIGH;
    delay_4us();
    SDA = I2C_LOW;
    delay_4us();
}

void I2C_stop()
{
    SDA = I2C_LOW;
    delay_4us();
    SCL = I2C_HIGH;
    delay_4us();
    SDA = I2C_HIGH;
    delay_4us();
}

void delay_4us()
{
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}

void delay_ms(uint ms)
{
    uchar i;
    uint j;

    for (j = ms; j > 0; j--)
    {
        for (i = 120; i > 0; i--)
            ;
    }
}