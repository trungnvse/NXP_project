#ifndef _SREC_H_
#define _SREC_H_
/*******************************************************************************
* Include
*******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
/*******************************************************************************
* Define
*******************************************************************************/
#define convert_4bit(h) ( ('0'<= h) && (h <= '9') ? h - '0' :\
(('A' <= h) && (h <= 'F')? h - 'A' + 10 : -1 ))

#define convert_8bit(h1,h2) ( (convert_4bit(h1) != -1) &&\
(convert_4bit(h2) != -1) ? (convert_4bit(h1) << 4 | convert_4bit(h2)) : -1)

#define convert_16bit(h1,h2,h3,h4) ((convert_8bit(h1,h2)) != -1 &&\
(convert_8bit(h3,h4) != -1) ?\
(convert_8bit(h1,h2) << 8 | convert_8bit(h3,h4)) : -1)

typedef enum Check_status{
    OK,
    ERROR_START,
    ERROR_TYPE,
    ERROR_COUNT_BYTE,
    ERROR_HEXA,
    ERROR_CHECKSUM,
    ERROR_COUNT_LINE,
    ERROR_TYPE_CONFLICT,
} Status_t_enum;

typedef enum srec_file_type{
    S19,
    S28,
    S37,
    INVALID,
} srec_file_type_t_enum;

typedef enum srec_line{
	line_data,
	line_check,
	line_end,
} srec_line_t_enum;

#define LENGHT_ADDRESS_S19    4
#define LENGHT_ADDRESS_S28    6
#define LENGHT_ADDRESS_S37    8
/*******************************************************************************
* API
*******************************************************************************/

/**
  * @brief                   check SREC
  * char* ptr_srec           pointer to SREC
  * @return value            status of SREC, if SREC has no error, return OK status
  */
Status_t_enum check_srec(char* ptr_srec);

/**
  * @brief                   check SREC
  * char* ptr_srec           pointer to SREC
  * char** ptr_data          pointer to pointer of data which will be parsed
  * char** ptr_address       pointer to pointer of address which will be parsed
  * uint16_t lenght_address  length of address
  * uint16_t length_data     length of data
  * @return value            if line SREC is line data(start record is S1,S2,S3),
  *                          return 'line_data', if not return 'line_check'
  */
srec_line_t_enum parse_srec(char* ptr_srec, char** ptr_data, char** ptr_address, uint16_t* lenght_address, uint16_t* length_data);

#endif /* _SREC_H_ */
/*******************************************************************************
* EOF
*******************************************************************************/
