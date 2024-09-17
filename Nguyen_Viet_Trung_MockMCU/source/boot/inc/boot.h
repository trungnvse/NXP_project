#ifndef _BOOT_H_
#define _BOOT_H_
/*******************************************************************************
* Include
*******************************************************************************/
#include "MKE16Z4.h"
/*******************************************************************************
* define
*******************************************************************************/

#define    address_start_app            0x8000 /* should fix = 0x4000 */
#define    address_start_backup         0xB000 /* if start app = 0x4000 => 0x9000*/
#define    address_check_status_boot    0xE000 /* if value in this address != 0, boot fail */
#define    address_save_totalbyte       0xEE00 /* this address save value of g_total_byte variable  */
#define    size_app_sector              10     /* 10 sector */   /* fix it if start app changed*/
#define    size_backup_sector           10     /* 10 sector */   /* fix it if start app changed*/

typedef enum Status_boot{
	QUEUE_OVERFLOW,
	DATA_INVALID,
	SUCESS,
} Status_boot_e_t;


typedef void( *callback_t)(void);
/**
  * @brief                   boot into flash
  * uint32_t baud_rate[in]   baud rate of UART
  * @return value            status of booting process
  */
Status_boot_e_t process_boot(const uint32_t baud_rate);

/**
  * @brief                   jump into application, doesn't boot
  * @return value            None
  */
void jump_application();

void back_up();

void roll_back();
/*******************************************************************************
* API
*******************************************************************************/



#endif /* _BOOT_H_ */
/*******************************************************************************
* EOF
*******************************************************************************/
