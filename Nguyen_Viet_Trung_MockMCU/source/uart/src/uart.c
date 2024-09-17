/*******************************************************************************
* Include
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "MKE16Z4.h"
#include "uart.h"
/*******************************************************************************
* Global variable
*******************************************************************************/
volatile char temp;
volatile callback g_callback_func;
/*******************************************************************************
* Prototype
*******************************************************************************/
/**
  * @brief                   Analyze to find the appropriate pair of numbers(OSR and SBR)
  * uint32_t _clock          clock for UART
  * uint32_t _baud_rate      baud rate for UART
  * uint16_t* _OSR           OSR value field of BAUD register
  * uint16_t* _SBR           SBR value field of BAUD register
  * @return value            None
  */
/*                         9600                            115200                          256000
 * cacul_baudrate        | success (OSR = 24; SBR =  200) |   fail (OSR = 2;  SBR =  139) |fail
 * cacul_baudrate_OSR32  | success (OSR = 31; SBR =  156) |success (OSR = 31; SBR =  13 ) |fail
 * */
void cacul_baudrate(uint32_t _clock, uint32_t _baud_rate, uint16_t* _OSR, uint16_t* _SBR);
void cacul_baudrate_OSR32(uint32_t _clock, uint32_t _baud_rate, uint16_t* _OSR, uint16_t* _SBR);
/*******************************************************************************
* Code
*******************************************************************************/


/*******************************************************************************
* layer 3  BOOT
*
* layer 2: QUEUE, SREC
*
* layer 1: UART,  FLASH
*
* NOTE:
* _ TẦNG DƯỚI KHÔNG ĐƯỢC GỌI TRỰC TIẾP HÀM CỦA TẦNG TRÊN
* _ KHÔNG SỬ DỤNG BIẾN TOÀN CỤC
*******************************************************************************/


void LPUART0_IRQHandler(void)
{
	temp = LPUART0->DATA & 0xFF;\
	if(temp != '\0')
	{
		g_callback_func(temp); /* TƯƠNG ĐƯƠNG VIỆC GỌI HÀM ADD_ELEMENT CỦA TẦNG QUEUE */
	}
}

void register_take_data_from_uart(callback _callback_func)
{
	if(_callback_func != NULL)
	{
		g_callback_func = _callback_func;
	}
}

void init_uart(const uint32_t _baud_rate)
{
	uint16_t OSR = 0;
	uint16_t SBR = 0;
	cacul_baudrate_OSR32(48000000, _baud_rate, &OSR, &SBR);
	/* clock for bus clock UART register is 24mhz */
	PCC->CLKCFG[PCC_LPUART0_INDEX] &= ~PCC_CLKCFG_CGC_MASK;
	PCC->CLKCFG[PCC_LPUART0_INDEX] |= PCC_CLKCFG_PCS(3);
	PCC->CLKCFG[PCC_LPUART0_INDEX] |= PCC_CLKCFG_CGC_MASK;

	/* clock for UART is 48mhz */
	SCG->FIRCDIV |= SCG_FIRCDIV_FIRCDIV2(1);

	/* configuration 9600 baud rate */
    /* OSR = 31, SBR = 156 */
	LPUART0->BAUD = (LPUART0->BAUD & ~ (0X1F << 24)) | (OSR << 24);
	//LPUART0->BAUD |= LPUART_BAUD_OSR(31);
	LPUART0->BAUD = (LPUART0->BAUD & ~ (0X1FFF)) | (SBR & 0X1FFF);
	//LPUART0->BAUD |= LPUART_BAUD_SBR(156);

	/* select 8 bit data*/
	LPUART0->BAUD &= ~(1 << 29);
	LPUART0->CTRL &= ~(1 << 11); //M7
	LPUART0->CTRL &= ~(1 << 4);  //M

	/* select none parity */
	LPUART0->CTRL &= ~(1 << 1);
	/* select bit stop */
	LPUART0->BAUD &= ~(1 << 13);

	/* enable port B for UART */
	PCC->CLKCFG[PCC_PORTB_INDEX] |= PCC_CLKCFG_CGC(1);
	/* PTB0 ~ RX */
	PORTB->PCR[0] = (PORTB->PCR[0] & (~ PORT_PCR_MUX_MASK )) | PORT_PCR_MUX(2);
	/* PTB1 ~ TX */
	PORTB->PCR[1] = (PORTB->PCR[1] & (~ PORT_PCR_MUX_MASK )) | PORT_PCR_MUX(2);

	/* enable transmit */
	LPUART0->CTRL |= LPUART_CTRL_TE(1);

	/* enable receiver */
	LPUART0->CTRL |= 1 << 18;
	/* enable interrupt */
	LPUART0->CTRL |= 1 << 21;
	NVIC_EnableIRQ(LPUART0_IRQn);

}



void send_character(char character)
{
	while(((LPUART0->STAT >> 23 ) & 0x01)  == 0);
	LPUART0->DATA =  (LPUART0->DATA & ~0xFF) | (character & 0XFF);
	//while(((LPUART0->STAT >> 23 ) & 0x01)  == 0);
}

void cacul_baudrate_OSR32(uint32_t _clock, uint32_t _baud_rate, uint16_t* _OSR, uint16_t* _SBR)
{
	*_OSR = 31;
	*_SBR = _clock/_baud_rate/32;
}

void cacul_baudrate(uint32_t _clock, uint32_t _baud_rate, uint16_t* _OSR, uint16_t* _SBR)
{
	uint32_t temp_baud_rate_1       = _clock;
	uint16_t temp_OSR_1             = 0;
	uint16_t temp_SBR_1             = 0;
	uint32_t temp_baud_rate_2       = 0;
	uint16_t temp_OSR_2             = 0;
	uint16_t temp_SBR_2             = 0;
	uint16_t index_OSR              = 0;
	uint16_t index_SBR              = 0;
	uint32_t temp_baud_rate_extra   = 0;


	for(index_OSR = 32; index_OSR  > 0; index_OSR--)
	{
		for(index_SBR = 1; index_SBR < 0x1FFF; index_SBR++)
		{
			temp_baud_rate_extra = _clock/(index_OSR + 1)/index_SBR;

			/* cận trên */
			if(temp_baud_rate_extra > _baud_rate)
			{
				if((temp_baud_rate_extra - _baud_rate) < (temp_baud_rate_1 - _baud_rate))
				{
					temp_OSR_1 = index_OSR;
					temp_SBR_1 = index_SBR;
					temp_baud_rate_1 = temp_baud_rate_extra;
				}
			}

			/* cận dưới */
			if(temp_baud_rate_extra < _baud_rate)
			{
				if((_baud_rate - temp_baud_rate_extra) < (_baud_rate - temp_baud_rate_2))
				{
					temp_OSR_2 = index_OSR;
					temp_SBR_2 = index_SBR;
					temp_baud_rate_2 = temp_baud_rate_extra;
				}
			}

			if(temp_baud_rate_extra == _baud_rate)
			{
				temp_OSR_2 = index_OSR;
				temp_SBR_2 = index_SBR;
				temp_baud_rate_2 = temp_baud_rate_extra;
				index_SBR = 0x1FFF;
				index_OSR = 1;
			}
		}
	}

	if((_baud_rate - temp_baud_rate_2) < (temp_baud_rate_1 - _baud_rate))
	{
		*_OSR = temp_OSR_2;
		*_SBR = temp_SBR_2;
	}else{
		*_OSR = temp_OSR_1;
		*_SBR = temp_SBR_1;
	}
	/* finish this function */
}
/*******************************************************************************
* EOF
*******************************************************************************/
