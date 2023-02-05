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
Renvoie un pointeur vers un tableau d'octets contenant le prochain morceau du fichier.
La longueur maximale de chaque morceau (en nombre d'octets) est définie par le paramètre CHUNCK_MAX_SIZE.
La taille du morceau est l'octet CHUNCK_MAX_SIZE s'il reste au moins CHUNCK_MAX_SIZE octets dans le fichier,
sinon, il contient les octets restants du fichier.
La longueur du morceau est écrite dans *len.
 */
u8_t * tftp_file_getbyte(void * pfile);

#endif /* TFTP_FILE_H_ */
