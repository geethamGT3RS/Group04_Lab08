#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdlib.h>
volatile int check = 0;
volatile int start = 0;
void Delay(unsigned long counter);
char UART0_Receiver(void);
void UART0_Transmitter(char data);
void UART0_INT_HANDLER(void)
{
    char rx;
    rx = UART0_Receiver();
    if (rx=='R')
    {
        GPIO_PORTF_DATA_R = 0x02;
    }
    else if (rx =='G')
    {
        GPIO_PORTF_DATA_R = 0x08;
    }
    else if (rx == 'B')
    {
        GPIO_PORTF_DATA_R = 0x04;
    }
    else{
        GPIO_PORTF_DATA_R = 0x00;
    }

    UART0_Transmitter(rx);
    UART0_ICR_R|=0x10;
}

int main(void)
{
    SYSCTL_RCGCUART_R |= 0x01; /* enable clock to UART0 */
    SYSCTL_RCGCGPIO_R |= 0x01; /* enable clock to PORTA for PE4/Rx and PE5/Tx */
    Delay(1);

    /* UART5 initialization */
    UART0_CTL_R = 0;     /* disable UART5 */
    UART0_IBRD_R = 104;  /* for 9600 baud rate, integer part = 104 */
    UART0_FBRD_R = 11;   /* for 9600 baud rate, fractional part = 11 */
    UART0_CC_R = 0;      /* use system clock */
    UART0_LCRH_R = 0x62; /* data length 8-bit, no parity, no FIFO */
    UART0_CTL_R = 0x301; /* enable UART5, Rx and Tx */
    UART0_IM_R |= 0x10;  // Enable RX interrupt



    /* UART5 Tx (PE5) and Rx (PE4) configuration */
    GPIO_PORTA_DEN_R = 0x03;        /* enable digital for PE4 and PE5 */
    GPIO_PORTA_AFSEL_R = 0x03;      /* enable alternate function for PE4 and PE5 */
    GPIO_PORTA_AMSEL_R = 0;         /* disable analog function for PE4 and PE5 */
    GPIO_PORTA_PCTL_R = 0x00000011; /* configure PE4 and PE5 for UART5 */

    SYSCTL_RCGC2_R |= 0x00000020;   /* enable clock to GPIOF */
    GPIO_PORTF_LOCK_R = 0x4C4F434B; /* unlock commit register */
    GPIO_PORTF_CR_R = 0x1F;         /* make PORTF0 configurable */
    GPIO_PORTF_DEN_R = 0x1F;        /* set PORTF pins 4 pin */
    GPIO_PORTF_DIR_R = 0x0E;        /* set PORTF4 pin as input user switch pin */
    GPIO_PORTF_PUR_R = 0x11;        /* PORTF4 is pulled up */
    NVIC_EN0_R |= (1 << 5);  // Enable UART0 interrupt in NVIC (IRQ number 5)
    Delay(1);
    int v = UART0_DR_R;
    while (1)
    {

    }
}

void UART0_Transmitter(char data)
{
    while ((UART0_FR_R & (1 << 5)) != 0)
        ;
    UART0_DR_R = data;
    check = check + 1;
}

char UART0_Receiver(void)
{
    char data;
    while ((UART0_FR_R & (1 << 4)) != 0)
        ;
    data = UART0_DR_R;
    start = start + 1;
    return data;
}
void Delay(unsigned long counter)
{
    unsigned long i = 0;
    for (i = 0; i < counter; i++)
        ;
}
