/*
 * tftp_server.c
 *
 *  Created on: 2 janv. 2023
 *      Author: ayman
 */

#include <tftp_server.h>
enum tftp_err tftp_server_init(void){
  MX_LWIP_Init();
  tftp_file_init();

  struct netconn *conn = netconn_new(NETCONN_UDP);
  if(netconn_bind(conn, NULL, 69) == ERR_OK){
		return TFTPSERV_OK;
  }
	else{
		return TFTPSERV_ERR;
  }
}
void tftp_server_run(void ){

}
