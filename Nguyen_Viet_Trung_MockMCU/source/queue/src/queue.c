/*******************************************************************************
* Include
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "MKE16Z4.h"
#include "queue.h"
#include "uart.h"
/*******************************************************************************
* Global variable
*******************************************************************************/
char     queue[4][256];
volatile uint16_t   index_queue            = 0;
volatile uint16_t   index_element          = 0;
volatile uint16_t   getting_index_queue    = 0;
volatile uint16_t   getting_index_element  = 0;
volatile uint8_t    numbersof_avai_queue   = 0;
volatile overflow_queue_e_t g_overflow_queue = NOT_OVERFLOW;
/*******************************************************************************
* Prototype
*******************************************************************************/
void add_element(char _element);
/*******************************************************************************
* Code
*******************************************************************************/

/*******************************************************************************
 *
 * +, DỮ LIỆU SẼ ĐƯỢC CHUYỀN "LIÊN TỤC" QUA UART
 *
* KHÔNG DÙNG QUEUE: CHỈ SỬ DỤNG MẢNG 1 CHIỀU
*
*
*  X = [     ]
*
*  DATA => UART => BOOT => KIỂM TRA => BỘ NHỚ (FLASH)
*
*  LẦN 1: [1,2,3] => X (LÚC NÀY X = [1,2,3]) => ĐANG KIỂM TRA(CHƯA LƯU VÀO BỘ NHỚ)
*  LẦN 2: [4,5,6] => X (LÚC NÀY X = [4,5,6]) => VẪN ĐANG KIỂM TRA (CHƯA LƯU)
*  LẦN 3: [7,8,9] => X (LÚC NÀY X = [7,8,9])
*
* DÙNG QUEUE(MẢNG 2 CHIỀU):
*
* X = [     ] [     ] [     ][     ]
*
*  LẦN 1: [1,2,3] => X (LÚC NÀY X = [1,2,3] [     ]  [     ]  [     ] ) => ĐANG KIỂM TRA(CHƯA LƯU VÀO BỘ NHỚ)
*  LẦN 2: [4,5,6] => X (LÚC NÀY X = [1,2,3] [4,5,6]  [     ]  [     ] ) => VẪN ĐANG KIỂM TRA (CHƯA LƯU)
*  LẦN 3: [7,8,9] => X (LÚC NÀY X = [1,2,3] [4,5,6]  [7,8,9]  [     ] ) => ĐÃ XONG, LƯU VÀO BỘ NHỚ [1,2,3]
*  LẦN 4: [a,b,c] => X (LÚC NÀY X = [1,2,3] [4,5,6]  [7,8,9]  [a,b,c] )
*  LẦN 5: [d,e,f] => X (LÚC NÀY X = [d,e,f] [4,5,6]  [7,8,9]  [a,b,c] )
*
**************************************************************************************************************
*
*POINTER -> LƯU ĐỊA CHỈ (MẢNG, HÀM)
*HÀM   -> ỨNG DỤNG VÀO CALLBACK FUNCTION -> KHÔNG CẦN BIẾT TÊN HÀM
*
*
*MẢNG  ->
*TH1:
*layer 3  BOOT     MẢNG X2 (MẤT THỜI GIAN SAO CHÉP TỪ MẢNG X1 VÀO MẢNG X2)
*
*layer 2  QUEUE    MẢNG X1
*
*layer 1  UART
*TH2:
*layer 3  BOOT     ĐỊA CHỈ CỦA MẢNG X (VẪN CHỈ CẦN THEO TÁC TRÊN MẢNG X)
*
*layer 2  QUEUE    MẢNG X
*
*layer 1  UART
*
*******************************************************************************/

void init_queue(const uint32_t _baud_rate)
{
	init_uart(_baud_rate);
	register_take_data_from_uart(add_element); /* LIÊN QUAN TỚI CALLBACK FUNCTION */
}

void add_element(char _element)
{
    if(_element == '\n')
    {
    	queue[index_queue][index_element] = _element;
    	if(index_queue == 3)
		{
			index_queue = 0;
		}else{
			index_queue++;
		}

		index_element = 0;
		numbersof_avai_queue++;
    }else{
    	queue[index_queue][index_element] = _element;
    	index_element++;
    }

    /* check if queue overflow */
    if(numbersof_avai_queue > 4)
    {
    	g_overflow_queue = OVERFLOW;
    }
}

char* get_queue(overflow_queue_e_t* _overflow_queue)
{
	char* current_queue = NULL;

	if(numbersof_avai_queue > 0)
	{
		current_queue = &(queue[getting_index_queue][0]);
		//numbersof_avai_queue--;
		if(getting_index_queue == 3)
		{
			getting_index_queue = 0;
		}else{
			getting_index_queue++;
		}
	}
	*_overflow_queue = g_overflow_queue;

	return current_queue;
}

void completed_process()
{
	numbersof_avai_queue--;
}
/*******************************************************************************
* EOF
*******************************************************************************/
