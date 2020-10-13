/*
 * File:   lib.c
 * Author: josem
 *
 * Created on October 11, 2020, 12:50 PM
 */


#include <xc.h>
#include <pic18f4550.h>
#include "../bsp/lib.h"

void init_timer0(void)
{
    INTCONbits.GIE = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0PS = 0b111;
    T0CONbits.T08BIT = 0;
    TMR0L = 46005;
    TMR0H = (46005) >> 8;
    T0CONbits.TMR0ON = 1;
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 0;      //DESACTIVADO HASTA QUE EMPIECE A LEER DATOS POR EL ADC
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    INTCON2bits.TMR0IP=0;
}


void init_adc(void)
{
    INTCONbits.GIE = 0;
    ADCON1bits.PCFG = 14;
    ADCON1bits.VCFG = 0;
    ADCON0=0;
    ADCON2bits.ACQT = 7;
    ADCON2bits.ADCS = 5;
    ADCON2bits.ADFM = 1;
    ADCON0bits.ADON=1;
    PIR1bits.ADIF=0;
    PIE1bits.ADIE=1;
    IPR1bits.ADIP=0;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    ADCON0bits.GO_DONE =1;
}


void init_int1(void)
{
    INTCONbits.GIE = 0;
    TRISBbits.TRISB1 = 1; 
    INTCON3bits.INT1IE = 1; 
    INTCON2bits.INTEDG1 = 0; 
    INTCON3bits.INT1IF = 0; 
    INTCON3bits.INT1IP=0;
    INTCONbits.PEIE = 1; 
    INTCONbits.GIE = 1; 
}