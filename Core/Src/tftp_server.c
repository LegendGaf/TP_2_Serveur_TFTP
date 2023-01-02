/*
 * tftp_server.c
 *
 *  Created on: 2 janv. 2023
 *      Author: ayman
 */

#include <tftp_server.h>
static struct netconn *tftp_server;
static struct netbuf *buf;
static ip_addr_t *addr;
static unsigned short port;
static u8_t *dataptr;
static u16_t len;


enum tftp_err tftp_server_init(void){
    MX_LWIP_Init();

    tftp_file_init();

    tftp_server = netconn_new(NETCONN_UDP);

    if(netconn_bind(tftp_server, NULL, 69) == ERR_OK)
        return TFTPSERV_OK;
    else
        return TFTPSERV_ERR;
}
void tftp_server_run(void ){
/* On charge les donnees recues dans un buffer "buf" */
    if(netconn_recv(tftp_server, &buf) == ERR_OK)
    {
        addr = netbuf_fromaddr(buf);	// On stock l'adresse IP avec laquelle on va communiquer dans "addr"
        port = netbuf_fromport(buf);	// On stock le port de communication dans "port"

        /* On stock la donnee du buffer dans un tableau "dataptr" */
        if(netbuf_data(buf, (void**)&dataptr, &len) == ERR_OK)
        {
            /* On verifie que la requete envoyee est correcte */
            if((dataptr+1) >= 1 && (dataptr+1) <=5)
            {
                /* On connecte le serveur à l'adresse IP "addr" via le port "port" */
                netconn_connect(tftp_server, addr, port);

                /* On traite la requete "RRQ" du client */
                respond_RRQ(dataptr);
            }
            //else log
        }
    }
}
