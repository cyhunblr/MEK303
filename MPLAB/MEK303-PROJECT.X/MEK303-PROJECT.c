
// PIC16F877A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#define _XTAL_FREQ 4000000
#define Sil 1
#define BasaDon 2
#define SolaYaz 4
#define SagaYaz 5
#define ImlecGizle 12
#define ImlecYanSon 15
#define ImlecGeri 16
#define KaydirSaga 24
#define KaydirSola 28
#define EkraniKapat 8
#define BirinciSatir 128
#define IkinciSatir 192
#define KarakterUretAdres 64
#define CiftSatir8Bit 56
#define CiftSatir4Bit 48
#define TekSatir8Bit 40
#define TekSatir4Bit 32
#define RS RB0
#define RW RB1
#define E RB2
#define Data PORTD

void Lcd_Port(char a)
{
    if(a & 1)
        RD4 = 1;
    else
        RD4 = 0;
    
    if(a & 2)
        RD5 = 1;
    else
        RD5 = 0;
    
    if(a & 4)
        RD6 = 1;
    else
        RD6 = 0;
    
    if(a & 8)
        RD7 = 1;
    else
        RD7 = 0;
}

void Lcd_Cmd(char a) // LCD komut fonksiyonu
{
    RS = 0;
    Lcd_Port(a);
    E = 1;
    __delay_ms(4);
    E = 0;
}

void Lcd_Clear() // LCD temizleme fonksiyonu
{
    Lcd_Cmd(0);
    Lcd_Cmd(1);
}

    void Lcd_Set_Cursor(char a, char b) // LCD satir sutün seçme fonksiyonu
{
    char temp,z,y;
    if(a == 1)
    {
        temp = 0x80 + b - 1;
        z = temp>>4;
        y = temp & 0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    }
    else if(a == 2)
    {
        temp = 0xC0 + b - 1;
        z = temp>>4;
        y = temp & 0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    }
}

void Lcd_Init()        // LCD baslatma fonksiyonu
{
    Lcd_Port(0x00);
    __delay_ms(20);
    Lcd_Cmd(0x03);
    __delay_ms(5);
    Lcd_Cmd(0x03);
    __delay_ms(11);
    Lcd_Cmd(0x03);
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x08);
    Lcd_Cmd(0x00);
    Lcd_Cmd(0x0C);
    Lcd_Cmd(0x00);
    Lcd_Cmd(0x06);
}

void Lcd_Write_Char(char a) // LCD ye karakter yazd?rma fonksiyonu
{
    char temp,y;
    temp = a&0x0F;
    y = a&0xF0;
    RS = 1;
    Lcd_Port(y>>4);
    E = 1;
    __delay_us(40);
    E = 0;
    Lcd_Port(temp);
    E = 1;
    __delay_us(40);
    E = 0;
}

void Lcd_Write_String(char *a) // LCD ye string yazdirma fonksiyonu
{
    int i;
    for(i=0;a[i]!='\0';i++)
    Lcd_Write_Char(a[i]);
}

int map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (int)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void duty_send( int deger)
{
    CCP1X= deger & 2 ;
    CCP1Y = deger & 1;
    CCPR1L= deger >>2 ;
}

void main(void) 
{
    float digital = 0.0; 
    float voltage_coefficent = 0.0048;    // 5V / 1024
    float voltage = 0.0;
    char char_digital[15];
    int speed = 0.0;
    int counter = 0;
    int rotate = 0;
    TRISA=0XFF;                           
    TRISB=0xC0;                           
    TRISC=0X00;
    TRISD=0X00; 
    
    PORTB=0X00;
    PORTC=0X00; 
    PORTD=0X00; 
    
    Lcd_Init(); 
    
    ADCON0bits.ADON  = 1; 
    ADCON1bits.PCFG0 = 0;
    ADCON1bits.PCFG1 = 0;

    ADCON1bits.PCFG2 = 0;
    ADCON1bits.PCFG3 = 0;
    ADCON1bits.ADFM  = 1; 
    
    while(1)
    {
        // giris
        if (counter == 0)
        {
            Lcd_Set_Cursor(1,2); 
            Lcd_Write_String("AHMET C. BILIR"); 
            Lcd_Set_Cursor(2,4);
            Lcd_Write_String("B200104029"); 
            __delay_ms(3000);
            Lcd_Clear();
            counter++;
        }
        //AN0
        ADCON0bits.CHS2=0;
        ADCON0bits.CHS1=0;
        ADCON0bits.CHS0=0;
        ADCON0bits.GO =1; 
        while(ADCON0bits.GO_nDONE); 
        
        //Voltage
        digital = ADRESH*256 + ADRESL;
        voltage = digital * voltage_coefficent;
        //sprintf(char_digital,"%0.2f",voltage);
        
        //HIZ
        Lcd_Set_Cursor(1,5);
        speed = map(voltage,0,4.9,0,100);
        if (speed < 1)
        {
            speed=0;
            digital=0;
        }
        sprintf(char_digital,"%d",speed); 
        Lcd_Write_String("HIZ: %"); 
        Lcd_Write_String(char_digital); 
        
        //PWM               // Tosc = 1/4MHz => 0,25uS 
        int dutty = 0;      //PWM PERIYODU = [(PR2)+1] x 4 x Tosc x TMR2Prescale
                            //PWM PERIYODU = 256 x 4 x 0,25uS x 4 = 1024uS => l.024mS 
                            //PWM Dutty Cycle = [CCPR1L:CCP1CON<5:4>] x Tosc x TMR2Prescale
                            //PWM Dutty Cycle = ? x 1uS 
        CCP1CON=0B00001111; // PWM MODE ACTIVE  
        T2CON = 0B00000101; // T2ON + 1:4 Prescale
        TMR2=0;   
        PR2=255;
        
        //Yön Tesip Etme
        if (PORTBbits.RB6 == 1)
        {
            while(PORTBbits.RB6 == 1);
            rotate = 1;
        }
        else if (PORTBbits.RB7 == 1)
        {
            while(PORTBbits.RB7 == 1);
            rotate = 2;
        }
        
        // DC Motor Durumu
        if (speed > 0)
        {
            // CCW
            if (rotate == 1)
            {
                PORTCbits.RC3 = 1;
                PORTCbits.RC5 = 1;
                PORTCbits.RC4 = 0;
                PORTCbits.RC6 = 0;
                duty_send(digital);
                Lcd_Set_Cursor(2,5);
                Lcd_Write_String("YON: ---");
            }
            //CW
            if (rotate == 2)
            {
                PORTCbits.RC3 = 0;
                PORTCbits.RC5 = 0;
                PORTCbits.RC4 = 1;
                PORTCbits.RC6 = 1;
                duty_send(digital);
                Lcd_Set_Cursor(2,5);
                Lcd_Write_String("YON: ---");
            }
        }
        else
        {
            Lcd_Clear();
            PORTCbits.RC3 = 0;
            PORTCbits.RC5 = 0;
            PORTCbits.RC4 = 0;
            PORTCbits.RC6 = 0;
            sprintf(char_digital,"%d",speed); 
            Lcd_Set_Cursor(1,5);
            Lcd_Write_String("HIZ: %"); 
            Lcd_Write_String(char_digital);
            Lcd_Set_Cursor(2,5);
            Lcd_Write_String("YON: ---");
        }
    }
    return;
}