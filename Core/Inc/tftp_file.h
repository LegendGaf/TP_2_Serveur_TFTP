/*
 * tftp_file.h
 *
 */


#ifndef TFTP_FILE_H_
#define TFTP_FILE_H_

#include "lwip/sys.h"

/**
 * Initialize the file system
 */
void tftp_file_init();

/**
 * pfilename must points to a string containing a file name with its full path
 * Returns a pointer to the named file if it exists, NULL otherwise
 */
void * tftp_file_open(char * pfilename);

/**
 * Returns a pointer to an octet array containing the next chunck of the file
 * The maximal length of each chunck (in number of octets) is set by the CHUNCK_MAX_SIZE parameter
 * The size of the chunck is CHUNCK_MAX_SIZE octet if at least CHUNCK_MAX_SIZE octets remain in the file,
 * otherwise, it contains the remaining octets from the file
 * The length of the chunck is written in *len
 */
u8_t * tftp_file_getbyte(void * pfile);

#endif /* TFTP_FILE_H_ */
