#ifndef _QUEUE_H_
#define _QUEUE_H_
/*******************************************************************************
* Include
*******************************************************************************/

/*******************************************************************************
* define
*******************************************************************************/
typedef enum overflow_queue{
	OVERFLOW,
	NOT_OVERFLOW,
} overflow_queue_e_t;
/*******************************************************************************
* API
*******************************************************************************/

/**
  * @brief                              take data from queue
  * overflow_queue_e_t* _overflow_queu  Indicates the status of the queue, overflow or not
  * @return value                       pointer to address of current queue
  */
char* get_queue(overflow_queue_e_t* _overflow_queue);

/**
  * @brief                   initialize queue
  * uint32_t _baud_rate [in] baud rate of UART
  * @return value            None
  */
void init_queue(const uint32_t _baud_rate);

/**
  * @brief                   free a queue, this queue can use to storage another data
  * @return value            None
  */
void completed_process();

#endif /* _QUEUE_H_ */
/*******************************************************************************
* EOF
*******************************************************************************/
