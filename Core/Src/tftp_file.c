/*
 * tftp_file.c
 *
 *  Created on: 29 oct. 2018
 *      Author: Vincent Van Assche
 */

/* For now : only one pseudo-file named "param" is readable
 ** It is in fact an array filled with alphabetical letters during init
 */
//test
#include <string.h>
#include "tftp_file.h"
#include "lwip/sys.h"

#define PARAM_FILE_SIZE 10	// To be modified when needed

static u8_t tftp_pseudo_file[PARAM_FILE_SIZE];
static uint current_index;

void tftp_file_init() {
	for (int i = 0; i < PARAM_FILE_SIZE; i++) {
		tftp_pseudo_file[i] = (u8_t) (i % 26 + 97);
	}
	current_index = 0;
}

void * tftp_file_open(char * pfilename) {
	if (strcmp("param", pfilename) == 0)
	{
		current_index = 0;
		return tftp_pseudo_file;
	}
	else
		return NULL;
}

u8_t * tftp_file_getbyte(void * pfile) {
	u8_t * byte;
	if ((pfile != NULL) && (current_index < PARAM_FILE_SIZE))
	{
		byte = pfile + current_index;
		current_index++;
	}
	else
	{
		byte = NULL;
	}
	return byte;
}
