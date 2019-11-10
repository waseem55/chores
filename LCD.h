/* 
 * File:   LCD.h
 * Author: waseem
 *
 * Created on October 16, 2019, 9:45 AM
 */

#ifndef LCD_H
#define	LCD_H
#endif
#define _XTAL_FREQ 31000

void lcd_cmd(unsigned char c);
void lcd_data(unsigned char d);
void lcd_init(void);
void enable(void);
void lcd_print(unsigned char *text, unsigned char size, unsigned char line, unsigned char pos);
void lcd_clear_all();
void lcd_clear_line(unsigned char line);

