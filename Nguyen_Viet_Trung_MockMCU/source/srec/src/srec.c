/*******************************************************************************
* Include
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "MKE16Z4.h"
#include "srec.h"
#include "uart.h"
/*******************************************************************************
* Global variable
*******************************************************************************/

/*******************************************************************************
* Prototype
*******************************************************************************/
void check_line(uint16_t cout_line, char* line, Status_t_enum* ptr_status);
Status_t_enum Check_count_line(uint16_t count_line,char* line);
uint16_t count_byte(char* rest_line);
Status_t_enum hex_to_dec(char** hex, uint8_t* dec);
Status_t_enum Check_type_record(char* line, uint16_t count_line);
/*******************************************************************************
* Code
*******************************************************************************/

/*******************************************************************************
* PARSE:
*
*
*
*******************************************************************************/


srec_line_t_enum parse_srec(char* ptr_srec, char** ptr_data, char** ptr_address, uint16_t* lenght_address, uint16_t* length_data)
{
	char               record_type       = ptr_srec[1];
	srec_line_t_enum   type_line         = line_data;

	if(ptr_srec != NULL)
	{
		* length_data  = count_byte(ptr_srec) - 6;
	}

	switch (record_type)
	{
		case '1':
			type_line = line_data;
			* ptr_address      = (ptr_srec + 4);
			* ptr_data         = (ptr_srec + 4 + LENGHT_ADDRESS_S19);
			* lenght_address   = LENGHT_ADDRESS_S19;
			* length_data      = * length_data - LENGHT_ADDRESS_S19;
			break;
		case '2':
			type_line = line_data;
			* ptr_address      = (ptr_srec + 4);
			* ptr_data         = (ptr_srec + 4 + LENGHT_ADDRESS_S28);
			* lenght_address   = LENGHT_ADDRESS_S28;
			* length_data      = * length_data - LENGHT_ADDRESS_S28;
			break;
		case '3':
			type_line = line_data;
			* ptr_address      = (ptr_srec + 4);
			* ptr_data         = (ptr_srec + 4 + LENGHT_ADDRESS_S37);
			* lenght_address   = LENGHT_ADDRESS_S37;
			* length_data      = * length_data - LENGHT_ADDRESS_S37;
			break;
		case '7':
			type_line = line_end;
			break;
		case '8':
			type_line = line_end;
			break;
		case '9':
			type_line = line_end;
			break;
		default:
			type_line = line_check;
			break;
	}

	return type_line;
}


/*******************************************************************************
* CHƯƠNG TRÌNH ỨNG DỤNG ->  FILE S-RECORD
* CHECK :
* 1 DÒNG S-RECORD
* | S | TYPE | BYTE COUNT | ADDRESS | DATA                             | CHECKSUM |
*   S     1       13          8000    00180020D58000004381000045810000     55
*******************************************************************************/


Status_t_enum check_srec(char* ptr_srec)
{
	Status_t_enum       status       = OK;
	static     uint16_t cout_line    = 0;

	check_line(cout_line, ptr_srec, &status);
	cout_line++;
	return status;
}

void check_line(uint16_t cout_line, char* line, Status_t_enum* ptr_status)
{
    uint8_t             bytecout       = 0;
    char*               rest_line      = line + 2;
    uint16_t            index          = 0;
    uint8_t             arr_temp[100];
    uint8_t             sum            = 0;
    uint8_t             check_sum      = 0;
    *ptr_status                        = OK;


    if(*line != 'S') /* check start record - CHECK TYPE */
    {
        *ptr_status = ERROR_START;

    }else{
        /* check type record - CHECK TYPE */
        *ptr_status = Check_type_record(line, cout_line);
        if( *ptr_status == OK )
        {
            if(hex_to_dec(&rest_line, &bytecout) == ERROR_HEXA)
            {   /* check hexadecimal  */
                *ptr_status = ERROR_HEXA;

            } else if(count_byte(rest_line) != (bytecout*2))
            {
                *ptr_status = ERROR_COUNT_BYTE;/* check byte count record - CHECK BYTE COUNT  */

            } else{
                rest_line      = &line[2];
                for(index = 0; index < bytecout; index++)
                {
                    if(hex_to_dec(&rest_line, &arr_temp[index]) == ERROR_HEXA)
                    {   /* check hexadecimal  */
                        index = 200;
                        *ptr_status = ERROR_HEXA;

                    }
                    sum += arr_temp[index];
                }

                if( ( *ptr_status == OK) \
                && (hex_to_dec(&rest_line, &check_sum) == ERROR_HEXA) )
                {
                    *ptr_status = ERROR_HEXA; /* check hexadecimal  */

                }


                if((*ptr_status == OK ) && (check_sum != (255-sum)))
                {
                    *ptr_status = ERROR_CHECKSUM;/* check sum record - CHECK SUM */

                }
            }
        }
    }

}



Status_t_enum Check_type_record(char* line, uint16_t count_line)
{
    uint8_t record_type = *(line+1);
    srec_file_type_t_enum srec_type_current = S19;
    static srec_file_type_t_enum srec_type_before  = S19;
    Status_t_enum relvar = OK;

    if( (count_line == 0) && (record_type != '0') )
    {
        relvar = ERROR_TYPE;

    }

    if(count_line > 0)
    {
        switch (record_type)
        {
    	case '1':
        case '9':
            srec_type_current = S19;
    	    break;
    	case '2':
    	case '8':
    	    srec_type_current = S28;
    		break;
    	case '3':
    	case '7':
    		srec_type_current = S37;
    		break;
    	case '5':
    	case '6':
    	    relvar = Check_count_line(count_line, line);
    	    srec_type_current = srec_type_before;
    	    break;
    	default:
    	    relvar = ERROR_TYPE;

    		break;
    	}

    	if( (count_line > 1) && (srec_type_current != srec_type_before))
        {
            relvar =  ERROR_TYPE_CONFLICT;

        } else{
            srec_type_before = srec_type_current;
        }
    }

    return relvar;
}


Status_t_enum hex_to_dec(char** hex, uint8_t* dec)
{
    Status_t_enum      relvar      = OK;
    char               character1  = 'A';
    char               character2  = 'B';
    int16_t            temp        = 0;

    character1 = **hex;
    *hex = *hex +1;
    character2 = **hex;
    *hex = *hex + 1;
    temp = convert_8bit(character1,character2);

    if(temp == -1)
    {
        relvar = ERROR_HEXA;
    } else{
        *dec = temp;
    }

    return relvar;
}



uint16_t count_byte(char* rest_line)
{
    uint16_t index = 0;

    while( rest_line[index] != '\r' )
    {
        index++;
    }

    return index;
}



Status_t_enum Check_count_line(uint16_t count_line,char* line)
{
    Status_t_enum relvar      = OK;
    uint16_t      count       = 0;
    uint8_t       part1       = 0;
    uint8_t       part2       = 0;
    char*      rest_line   = line + 4;
    static uint16_t count_line_s5_s6 = 0;

    relvar = hex_to_dec(&rest_line, &part1);
    relvar = hex_to_dec(&rest_line, &part2);
    count = part1 << 8 | part2;

    if( count != count_line - 1 - count_line_s5_s6)
    {
        relvar = ERROR_COUNT_LINE;

    }

    count_line_s5_s6++;
    return relvar;
}
/*******************************************************************************
* Code
*******************************************************************************/

/*******************************************************************************
* EOF
*******************************************************************************/
