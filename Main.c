/*Plan:
 * one interrupt in RB0 -> wakes device up and clears then prints data
 * start timer and poll input pins(task done, check users' scores, task failed)
 * when timer overflows -> clear display and turn it off, sleep
 */

/*
 * File:   LEDtest.c
 * Author: wasee
 *
 * Created on August 3, 2019, 9:59 PM
 */

// Internal External Switchover bit (Internal External Switchover mode enabled)
// CONFIG1
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode enabled)

#include <xc.h>
#include <pic.h>
#include <pic16f88.h>
#include "LCD.h"


#define NUMBER_OF_CHORES 11
int rand = 0;
int space_pos = 16;
unsigned char btn_pressed = 0;
unsigned char lcd_off = 0;
unsigned char user = 0;
unsigned char lastrb = 0;
const char users[2][7] = {"Waseem","Hiba"};
const char chores[NUMBER_OF_CHORES][30] = {"Vacuum","Clean bathroom","Mop floor","Wash bed sheets",
"Wash curtains","Dusting","Clean microwave","Scrub doors and cabinets",
"Organize closet","Clean under sinks","Clean fridge"};

unsigned char *text;
unsigned char long_text;
unsigned char scores[] = {0, 0};
char* itoa (int x);
char random(void);
unsigned char strlen(unsigned char string[]);

void main()
{
    TRISA = 0xc0;
    TRISB = 0x01;
    PORTA = 0x00;
    PORTB = 0x00;
    
    INT0IF = 0;     // Clear RB0 interrupt flag
    INT0IE = 1;     // Enable RB0 interrupt
    T0CS = 0;       // Setting timer0 source to internal clock
    PSA = 0;        // Setting prescalar for TMR0
    PS2 = 1;        // Prescalar value PS(2-0)
    PS1 = 1;        //
    PS0 = 1;        //
    TMR0IE = 0;     // Disable timer0 interrupt
    
    GIE = 1;        // Enable interrupts

    
    lcd_init();
    
    rand = (random()%10);
    text = &chores[rand % NUMBER_OF_CHORES][0];
    
    while (1){
        lcd_clear_all();
        if (strlen(text) > 16)
            {
                for (space_pos = 16; text[space_pos] != ' ' && space_pos != 0; space_pos--);
                long_text = 1;
            }
        lcd_print(text, space_pos, 0, 0);
        lcd_print(&users[user][0], 6, 1, 0);
        if (strlen(text) > 16)
        {
            __delay_ms(300);
            lcd_clear_line(0);
            lcd_print(text+space_pos+1, 16, 0, 0);
        }
        if (btn_pressed)
        {
            rand = (random()%10);
            text = &chores[rand % NUMBER_OF_CHORES][0];
            user = rand%2;
            btn_pressed = 0;
        }
        __delay_ms(1000);

        
    }
}

void __interrupt() testing(void)
{
    if (INT0IF == 1)
    {
        if (lcd_off)
        {
            lcd_cmd(0x0c);      // Turn LCD on
            lcd_clear_all();
            __delay_ms(50);     // Simple debounce
            lcd_off = 0;
        }
        else
            btn_pressed = 1;
        TMR0 = 0;
        TMR1 = 0;
        INT0IF = 0;
    }
    else if (TMR0IF)
    {
        lcd_off = 1;
        lcd_clear_line(0);
        lcd_print("sleep time!", 16, 0, 0);
        __delay_ms(500);
        lcd_cmd(0x08);      // Turn LCD off
        TMR0IF = 0;
        SLEEP();
    }
}


char* itoa (int x)
{
    int original = x;
    char buffer[8];
    
    int c = sizeof(buffer)-1;
    
    buffer[c] = 0;
    
    if (x < 0)
        x = -x;
    
    do{
        buffer[--c] = (x%10) + '0';
        x /= 10;
    }while (x);
    
    if (original < 0)
        buffer[--c] = '-';
    return &buffer[c];
}


char random(void)
{
    static char rand;
    static char count = 0;
    char original = rand;
    char rbit = 0;     //using bits from x to xor and then
                                //shift in right. 180 = 1011 0100
    if (count == 0){             // used to seed the generator with the timer
        while (RA7 == 0){            //only once
        }
        count = 1;
        rand = TMR0;
        TMR0 = 0;
        TMR0IE = 1;
        return rand;
    }
    
    original &= 180;                   
    while(original){
        rbit ^= (original & 1);
        original >>= 1;
        original &= (~(1<<7));
    }
    rand <<= 1;
    rand |= rbit;           //shifting in the xor result
    return rand;
}

unsigned char strlen( unsigned char string[])
{
    int i = 0;
    unsigned char result = 0;
    while (string[i] != 0)
    {
        i++;
        result++;
    }
    return result;
}