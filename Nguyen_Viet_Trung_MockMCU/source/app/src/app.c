/*******************************************************************************
* Include
*******************************************************************************/
#include "boot.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "MKE16Z4.h"
#include "app.h"
/*******************************************************************************
* Property
*******************************************************************************/
void init_Led();
void init_button();
void turn_on_led_green();
void turn_on_led_red();
void turn_on_led_yellow();
void turn_on_led_blue();
/*******************************************************************************
* Global variable
*******************************************************************************/
volatile uint8_t option       = 0;
/*******************************************************************************
* Code
*******************************************************************************/
int main(void)
{
	Status_boot_e_t status_boot = SUCESS;
#if 1
	init_Led();
	init_button();

	/* if the button is pressed when resetting, jump into boot mode:
	 * The red light is on, after successfully loading the code, the light turns green
	 * reset again, but don't press button, application will run */

	/* Should keep button (SW_3_PIN) before press reset button
	 * Keep press SW_3_PIN button until red led on to make sure the SW_3_PIN button pressed when resetting*/

	if((GPIOD->PDIR & (SW_3_PIN)) == 0)
	{
		option = 1;
	}

	if(option == 1)
	{
		turn_on_led_red();
		/* The transferred application should only be saved in 10 sectors (address) */
		status_boot = process_boot(115200);
		if(status_boot == SUCESS)
		{
			/* if booted successfully, turn green led */
			turn_on_led_green();
			while(1)
			{
				/* press button again if want to create a back up version for this version */
				if((GPIOD->PDIR & (SW_3_PIN)) == 0)
				{
					turn_on_led_red();
					back_up();
					turn_on_led_green();
					while(1);
				}
			}
		}else if(status_boot == QUEUE_OVERFLOW){
			/* if booted failed(queue overflowed ~ can't take data), turn yellow led */
			turn_on_led_yellow();
		}else{
			/* if booted failed(data is invalid), turn blue led */
			turn_on_led_blue();
		}

	}else{
		jump_application();
	}

#endif
	while(1)
	{

	}

    return 0;
}


void init_button()
{
	/* Enable Clock for PORTD */
	PCC->CLKCFG[PCC_PORTD_INDEX] |= PCC_CLKCFG_CGC(1);

	/* Setup PORTD_PIN2 as GPIO */
	PORTD->PCR[2] |= PORT_PCR_MUX(1);

	/* Setup PORTD_PIN2, PINB5 as input */
	GPIOD->PDDR &= ~(SW_3_PIN);

	/*  Interrupt on falling-edge */
	//PORTD->PCR[2] |= PORT_PCR_IRQC(10);
	//PORTD->PCR[2] |= PORT_PCR_IRQC(8);
	//PORTD->PCR[2] |= PORT_PCR_IRQC(12);
	/* enable interrupt */
	//NVIC_EnableIRQ(PORTBCD_IRQn);
	//NVIC->ISER[0] = (1 <<26);
}

void init_Led()
{
    /* Enable Clock for PORTB, PORTD */
	PCC->CLKCFG[PCC_PORTB_INDEX] |= PCC_CLKCFG_CGC(1);
	PCC->CLKCFG[PCC_PORTD_INDEX] |= PCC_CLKCFG_CGC(1);

    /* Setup PIND1, PINB4, PINB5 as GPIO */
    PORTD->PCR[1] |= PORT_PCR_MUX(1);
    PORTB->PCR[4] |= PORT_PCR_MUX(1);
    PORTB->PCR[5] |= PORT_PCR_MUX(1);

    /* Setup PORTD_PIN1, PORTB_PINB4, PINB5 as output */
    FGPIOB->PDDR |= GREEN_LED_PIN;
    FGPIOB->PDDR |= RED_LED_PIN;
    FGPIOD->PDDR |= BLUE_LED_PIN;

    /* Set default value for PORTD_PIN1, PORTB_PINB4, PINB5 */
    FGPIOB->PDOR |= GREEN_LED_PIN;
    FGPIOB->PDOR |= RED_LED_PIN;
    FGPIOD->PDOR |= BLUE_LED_PIN;
}

void turn_on_led_green()
{
    FGPIOB->PDOR |= RED_LED_PIN;
    FGPIOD->PDOR |= BLUE_LED_PIN;
	FGPIOB->PDOR &= ~GREEN_LED_PIN;
}

void turn_on_led_red()
{
    FGPIOB->PDOR |= GREEN_LED_PIN;
    FGPIOD->PDOR |= BLUE_LED_PIN;
	FGPIOB->PDOR &= ~RED_LED_PIN;
}

void turn_on_led_blue()
{
    FGPIOB->PDOR |= GREEN_LED_PIN;
    FGPIOD->PDOR &= ~BLUE_LED_PIN;
	FGPIOB->PDOR |= RED_LED_PIN;
}

void turn_on_led_yellow()
{
    FGPIOB->PDOR &= ~GREEN_LED_PIN;
    FGPIOD->PDOR |= BLUE_LED_PIN;
	FGPIOB->PDOR &= ~RED_LED_PIN;
}

/*******************************************************************************
* EOF
*******************************************************************************/
