#ifndef _UART_H_
#define _UART_H_
/*******************************************************************************
* Include
*******************************************************************************/

/*******************************************************************************
* Define
*******************************************************************************/
typedef void (*callback)(char _argument);


/*******************************************************************************
* API
*******************************************************************************/

/**
  * @brief                   initialize module UART
  * uint32_t baun_rate[in]   baud rate of UART
  * @return value            None
  */
void init_uart(const uint32_t baud_rate);

/**
  * @brief                   send character to other device through UART
  * char character           character will be sent
  * @return value            None
  */
void send_character(char character);

/**
  * @brief                   register callback function will be called
  * callback _callback_func  callback function will be called
  * @return value            None
  */
void register_take_data_from_uart(callback _callback_func);

#endif /* _UART_H_ */
/*******************************************************************************
* EOF
*******************************************************************************/
