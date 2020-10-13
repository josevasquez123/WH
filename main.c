/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/

/** INCLUDES *******************************************************/
#include "system.h"
#include <stdbool.h>
#include <stdint.h>
#include <pic18f4550.h>

#include "../../bsp/lib.h"
#include "usb.h"
#include "usb_device_hid.h"

#include "app_device_custom_hid.h"
#include "app_led_usb_status.h"



#define _XTAL_FREQ 20000000L


/*************VARIABLES GLOBALES******************/

volatile USB_HANDLE USBOutHandle;    
volatile USB_HANDLE USBInHandle;

unsigned char ReceivedDataBuffer[64] HID_CUSTOM_OUT_DATA_BUFFER_ADDRESS;
unsigned char ToSendDataBuffer[64] HID_CUSTOM_IN_DATA_BUFFER_ADDRESS;

volatile float adc_ppm=0.0;
volatile uint8_t counter=0;
volatile bool flag_ppm=0;
volatile bool flag_adc_end=0;
volatile bool flag_start_5s=0;
volatile bool flag_timer2=0;
volatile bool flag_start=0;
volatile uint8_t timer,timer2=0;
volatile uint8_t ppm=0;

PROGRAMA main(void)
{
    USB_Init(USB_ESTADO_START);
    
    USBDeviceInit();
    USBDeviceAttach();
    
    init_timer0();
    init_adc();
    init_int1();
    
    TRISD = 0;
    TRISA = 1;
    
    float adc_ppm_last=0.0;

    while(1)
    {
        
        if(flag_start)
        {
            if(flag_adc_end) 
            {
                if((adc_ppm !=adc_ppm_last) && (adc_ppm!=0.0))
                {
                    adc_ppm_last = adc_ppm;
                    flag_adc_end=0;
                    int volt2=0;
                    ToSendDataBuffer[0]=(int)adc_ppm;
                    volt2=(adc_ppm - (int)adc_ppm)*100;
                    ToSendDataBuffer[1]=volt2;
                    USBInHandle = (volatile USB_HANDLE)HIDTxPacket(CUSTOM_DEVICE_HID_EP,(uint8_t*)&ToSendDataBuffer[0],64);
                }
            }
            
            if(flag_ppm)
            {
                INTCON3bits.INT1IE = 0;
                INTCONbits.TMR0IE = 0;
                ppm=4*ppm;
                ToSendDataBuffer[2]=ppm;
                USBInHandle = (volatile USB_HANDLE)HIDTxPacket(CUSTOM_DEVICE_HID_EP,(uint8_t*)&ToSendDataBuffer[0],64);
                ppm=0;
                flag_ppm=0;
                INTCON3bits.INT1IE = 1;
                INTCONbits.TMR0IE = 1;      //ACTIVAR TIMER
                TMR0L = 46005;
                TMR0H = (46005) >> 8;
            }
            
        }
        
//        switch(ReceivedDataBuffer[0]){
//            case 0:
//                LATDbits.LATD7=0;
//                break;
//            case 1:
//                LATDbits.LATD7=1;
//                break;
//        }
       // USBOutHandle = (volatile USB_HANDLE)HIDRxPacket(CUSTOM_DEVICE_HID_EP,(uint8_t*)&ReceivedDataBuffer[0],64);
     //   USBInHandle = (volatile USB_HANDLE)HIDTxPacket(CUSTOM_DEVICE_HID_EP,(uint8_t*)&ToSendDataBuffer[0],64);
    }//end while
}//end main
/*******************************************************************************
 End of File
*/

void interrupt low_priority ISR(void)
{
    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF)
    {
        if(flag_start_5s)
        {
            if(timer2==5)
            {
                flag_timer2=1;
                timer2=0;
            }
            else
                timer2++;
        }
        
        if (timer==15)
        {
            timer=0;
            flag_ppm=1;
        }
        else
            timer++;
            
        TMR0L = 46005;
        TMR0H = (46005) >> 8;
        INTCONbits.TMR0IF = 0;
    }
    
    if(PIR1bits.ADIF==1)
    {
        adc_ppm = ADRES;
        adc_ppm = (adc_ppm*5.0)/1023;
        flag_adc_end=1;
        if(adc_ppm>0.6)
        {
            flag_start_5s=0;
            
            if(INTCONbits.TMR0IE==0)
              {
                flag_start=1;
                INTCONbits.TMR0IE = 1;      //ACTIVAR TIMER
                TMR0L = 46005;
                TMR0H = (46005) >> 8;
              }
        }
        else
        {
            flag_start_5s=1;
            
            if(flag_timer2==1)
            {
                flag_start=0;
                INTCONbits.TMR0IE = 0;      //DESACTIVAR TIMER  
                flag_timer2=0;
            }
            
            adc_ppm-0;
        }
        PIR1bits.ADIF=0;
        ADCON0bits.GO_DONE =1;
    }
    
    if (INTCON3bits.INT1IF)
    {   
        ppm++;
        INTCON3bits.INT1IF = 0;
    }
}
