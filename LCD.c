#include <xc.h>
#include <pic.h>
#include <pic16f88.h>
#include "LCD.h"




#define En RB2
#define D4_D7 PORTA
#define RS RB1

void lcd_print(unsigned char *text, unsigned char size, unsigned char line, unsigned char pos)
{
    // Doesn't check string size!
    // Size of line in memory = 40 chars
    // Size of displayed chars = 16 per line
    unsigned char *pointer = text;
    lcd_cmd((0x80) | (line << 6) | (pos)); // set cursor to position pos
    while ((*pointer) && (size != 0)){
        lcd_data(*pointer);
        pointer++;
        size--;
    }
}

void lcd_clear_all()
{
    lcd_cmd(0x01);
    __delay_ms(5);
}

void lcd_clear_line(unsigned char line)
{
    int i;
    lcd_cmd((line | 2) << 6); // set cursor to 0x00 or 0x40 (cmd for 0x00 is 0x80)
    for (i=0; i<40; i++)
    {
        lcd_data(' ');
    }
    lcd_cmd(0x80);
}

void lcd_init(void)
{
    // LCD initial initialization 

    __delay_ms(15);

    lcd_cmd(0x02);  // choose 4-bit interface
    __delay_ms(5);

    // LCD command setting initialization

    lcd_cmd(0x28); // 4 bit interface length. 2 rows enabled.
    __delay_ms(5);

    lcd_cmd(0x10); // Move cursor or shift display
    __delay_ms(5);

    lcd_cmd(0x0c); // Enable display. Cursor off.
    __delay_ms(5);

    lcd_cmd(0x06); // Increment cursor position after each byte 
    __delay_ms(5);

    lcd_cmd(0x01); // clear display
    __delay_ms(5);

    lcd_cmd(0x80); // Setting cursor position to 1st location
    __delay_ms(5);
}

void lcd_cmd(unsigned char c)
{
    // Send a command to LCD
    RS = 0;
    __delay_ms(5);
    //RD7 = 0;
    //RD6 = 0;
    //PORTC = (PORTC&0x0f)|(0xf0&c);
    D4_D7 = ((D4_D7 & 0xf0) | (0xf0 & c) >> 4);
    enable();
    //PORTC = ((PORTC&0x0f)|(0x0f&c)<<4);
    D4_D7 = (D4_D7 & 0xf0) | (0x0f & c);
    enable();
    __delay_ms(5);
}

void lcd_data(unsigned char d)
{
    // Send one character to LCD
    RS = 1;
    //RD7 = 1;
    //RD6 = 0;

    //PORTC = (PORTC&0x0f)|(0xf0&d);
    D4_D7 = ((D4_D7 & 0xf0) | (0xf0 & d) >> 4);
    enable();
    //PORTC = ((PORTC&0x0f)|(0x0f&d)<<4);
    D4_D7 = (D4_D7 & 0xf0) | (0x0f & d);
    enable();

}

void enable(void)
{
    En = 1;
    //RD5 = 1;
    __delay_ms(5);
    //RD5 = 0;
    En = 0;
}
