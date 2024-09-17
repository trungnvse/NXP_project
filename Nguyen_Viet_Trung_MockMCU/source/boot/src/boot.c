/*******************************************************************************
* Include
*******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "MKE16Z4.h"

#include "queue.h"
#include "srec.h"
#include "flash.h"
#include "boot.h"
/*******************************************************************************
* Property
*******************************************************************************/

/*******************************************************************************
* Global variable
*******************************************************************************/
callback_t g_reset                  = NULL;
volatile uint32_t   g_total_byte    = 0;
/*******************************************************************************
* Code
*******************************************************************************/
void jump_application()
{

	/* check if boot flash fail */
	if(Read_FlashAddress(address_check_status_boot) != 0xFFFFFFFF)
	{
		/* copy application from back up version to official application */
		roll_back();
	}

	/* disable all interrupt */
	__disable_irq();
	/* set vectors table */
	SCB->VTOR = (uint32_t)(address_start_app);
	/* set stack pointer */
	__set_MSP(*(uint32_t*)(address_start_app));
	/* set reset handle */
	g_reset = (callback_t)*(uint32_t*)(address_start_app + 4);
	/* jump */
	g_reset();
}


Status_boot_e_t process_boot(const uint32_t baud_rate) {

	Status_boot_e_t    relvar = SUCESS;
    uint16_t   index          = 0;
    /* local variable used when parse and check s-record */
    char*      ptr_srec       = NULL;
    char*      ptr_data       = NULL;
    char*      ptr_address    = NULL;
    uint16_t   lenght_address = 0;
    uint16_t   lenght_data    = 0;

    overflow_queue_e_t   overflow_queue = NOT_OVERFLOW;
    srec_line_t_enum     type_line      = line_check;
    /* local variable used when write into flash */
	int16_t    data           = 0;
	int32_t    address        = 0;
    char       arr[]          = {1,2,3,4};

    /* initialize queue and module UART */
    init_queue(baud_rate);

    /* erase 31 sector (31kb) in flash from address 0x8000 <application area> */
    Erase_Multi_Sector(address_start_app, size_app_sector);
    Program_LongWord(address_check_status_boot, 0xEE);

    /* take data from queue and boot into flash */
    while((type_line != line_end) && (relvar == SUCESS)) {
#if 1
    	/* take data from queue */
    	ptr_srec    = get_queue(&overflow_queue); /* LẤY ĐỊA CHỈ CỦA PHẦN TỪ ĐẦU TIÊN CỦA MẢNG */

    	/* check queue, if it is valid, continue processing */
    	if((ptr_srec != NULL) && (overflow_queue == NOT_OVERFLOW))
    	{
    		/* check data, if OK, data is valid, if not OK return error */
    		if(OK == check_srec(ptr_srec))
    		{
    			type_line = parse_srec(ptr_srec, &ptr_data, &ptr_address, &lenght_address, &lenght_data);
        		if( type_line == line_data)
        		{
        			g_total_byte = g_total_byte + lenght_data/2;

        			/* parse data from character ASII to hex */
        			address     = convert_16bit( (*ptr_address), (*(ptr_address + 1)), (*(ptr_address + 2)), (*(ptr_address + 3)));

        			for(index = 0; index < lenght_data; index = index + 8)
        			{
            			data        = convert_8bit(* (ptr_data + index), * (ptr_data + index + 1));
            			arr[0]      = data;
            			data        = convert_8bit(* (ptr_data + index + 2), * (ptr_data + index + 3));
            			arr[1]      = data;
            			data        = convert_8bit(* (ptr_data + index + 4), * (ptr_data + index + 5));
            			arr[2]      = data;
            			data        = convert_8bit(* (ptr_data + index + 6), * (ptr_data + index + 7));
            			arr[3]      = data;

            			if(address != -1)
    					{
            				/* write data into flash */
    						Program_LongWord_8B(address, (uint8_t*)arr);
    					}
            			address = address + 4;

        			}
        		}
    		}else{
    			/* if data is invalid, return error */
    			relvar = DATA_INVALID;
    		}
    		/* after process a queue, indicates this queue is free, can use */
    		completed_process();
    	}else if(overflow_queue == OVERFLOW){
    		/* if queue over flow, return error */
    		relvar = QUEUE_OVERFLOW;
    	}else{
    		/* if can't take queue(ptr_srec == NULL) and it doesn't overflow, queue haven't data*/
    	}

#endif
    }
    /* finish boot into flash */
    if(relvar != SUCESS)
    {

    }else{
    	Erase_Sector(address_check_status_boot);
    }
    return relvar;
}




void roll_back()
{
	uint32_t   address_backup   = address_start_backup;
	uint32_t   address_app      = address_start_app;
	uint16_t   index            = 0;
	uint32_t   data             = 0;
	uint32_t   total_byte       = 0;

	total_byte       = Read_FlashAddress(address_save_totalbyte);
	Erase_Multi_Sector(address_start_app, size_app_sector);
	for(index = 0; index < total_byte; index = index + 4)
	{
		data = Read_FlashAddress(address_backup);
		Program_LongWord(address_app, data);
		address_backup  = address_backup + 4;
		address_app     = address_app + 4;

	}
}

void back_up()
{
	uint32_t   address_backup   = address_start_backup;
	uint32_t   address_app      = address_start_app;
	uint16_t   index            = 0;
	uint32_t   data             = 0;

	Erase_Sector(address_save_totalbyte);
	Program_LongWord(address_save_totalbyte, g_total_byte);

	Erase_Multi_Sector(address_start_backup, size_backup_sector);
	for(index = 0; index < g_total_byte; index = index + 4)
	{
		data = Read_FlashAddress(address_app);
		Program_LongWord(address_backup, data);
		address_backup  = address_backup + 4;
		address_app     = address_app + 4;
	}
}



/*******************************************************************************
* EOF
*******************************************************************************/
