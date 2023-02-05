/*
 * tftp_server.c
 *
 *  Created on: 2 janv. 2023
 *      Author: ayman
 */

#include <tftp_server.h>

enum tftp_err tftp_server_init(void) {
    MX_LWIP_Init();

    tftp_file_init();

    tftp_server = netconn_new(NETCONN_UDP);

    if (netconn_bind(tftp_server, NULL, 69) == ERR_OK)
        return TFTPSERV_OK;
    else
        return TFTPSERV_ERR;
}

void tftp_server_run(void) {
/* On charge les donnees recues dans un buffer "buf" */
    if (netconn_recv(tftp_server, &buf) == ERR_OK) {
        addr = netbuf_fromaddr(buf);    // On stock l'adresse IP avec laquelle on va communiquer dans "addr"
        port = netbuf_fromport(buf);    // On stock le port de communication dans "port"

        /* On stock la donnee du buffer dans un tableau "dataptr" */
        if (netbuf_data(buf, (void **) &dataptr, &len) == ERR_OK) {
            /* On verifie que la requete envoyee est correcte */
            if (check_opcode(dataptr).code_validate == TFTPOP_OK) {
                /* On connecte le serveur à l'adresse IP "addr" via le port "port" */
                netconn_connect(tftp_server, addr, port);

                /* On traite la requete "RRQ" du client */
                respond_RRQ(dataptr);
            }
            //else log
        }
    }
}

struct opCode check_opcode(u8_t *data) {
    struct opCode opcode;

    /* We check the second Byte of the opcode since the first is always '0x00' */
    switch (*(data + 1)) {
        case 0x01: { // RRQ
            opcode.tftp_code = TFTPOP_RRQ;
            opcode.code_validate = TFTPOP_OK;
            break;
        }
        case 0x02: { // WRQ
            opcode.tftp_code = TFTPOP_WRQ;
            opcode.code_validate = TFTPOP_OK;
            break;
        }
        case 0x03: { // DATA
            opcode.tftp_code = TFTPOP_DATA;
            opcode.code_validate = TFTPOP_OK;
            break;
        }
        case 0x04: { // ACK
            opcode.tftp_code = TFTPOP_ACK;
            opcode.code_validate = TFTPOP_OK;
            break;
        }
        case 0x05: { // ERR
            opcode.tftp_code = TFTPOP_ERR;
            opcode.code_validate = TFTPOP_NOK;
            break;
        }
        default: {
            opcode.code_validate = TFTPOP_NOK;
            break;
        }
    }

    return opcode;
}


void respond_RRQ(u8_t *data) {
    enum tftp_operation tftp_code = check_opcode(data).tftp_code;

    char *filename;
    void *file;
    u8_t *file_bytes;

    if (tftp_code == TFTPOP_RRQ && flag_client == 1) {
        filename = malloc(sizeof(char));
        file_bytes = malloc(sizeof(u8_t));
        dataToSend = malloc(sizeof(u8_t));

        // Signal that the server is treating a RRQ and that any other RRQ should wait
        flag_client = 0;

        // Copy the filename within the RRQ packet in "filename"
        strcpy(filename, data + 2);

        // Counts the blocks sent
        block_counter = 1;

        if ((file = tftp_file_open(filename)) != NULL) {
            // The DATA to send "dataToSend" to the client
            dataToSend[0] = 0;
            dataToSend[1] = 3;
            dataToSend[2] = block_counter >> 8;
            dataToSend[3] = block_counter;

            // Counts the bytes within the file "param"
            byte_counter = 0;

            // Copy the data from "param" to "dataToSend"
            while ((file_bytes = tftp_file_getbyte(file)) != NULL) {
                *(dataToSend + 4 + byte_counter) = *file_bytes;
                byte_counter++;
            }

            // Signals the number of blocks of 512 Bytes within the "param" file
            blocksToSend = ((byte_counter) / 513) + 1;

            // Free the buffer so we can receive new data in it
            netbuf_delete(buf);

            // Move "dataToSend" in "buf->ptr->payload"
            if (blocksToSend == 1) // DATA to send <= 512 Bytes
            {
                // Move all data to the buffer "buf"
                netbuf_ref(buf, dataToSend, byte_counter + 4);
            } else if (blocksToSend > 1) // DATA to send > 512 Bytes
            {
                // Move 516 Bytes of data to the buffer: 2(opcode)+2(block_counter)+512(dataToSend)
                netbuf_ref(buf, dataToSend, 516);
            }

            // Send the buffer using the netconn "tftp_server"
            netconn_send(tftp_server, buf);
        }
    }

        /*
         * The server should already be responding to a request: flag_client = 0
         * The ACK operation for receiving data
         * The block_counter of data should be the same as the block_counter of the ACK
         */
    else if (tftp_code == TFTPOP_ACK && flag_client == 0
             && *(data + 2) == (block_counter >> 8) && *(data + 3) == block_counter) {
        netbuf_delete(buf);

        // Increment the block_counter
        block_counter++;

        // Move the 512 Bytes of "dataToSend" concerned for this transfer to make treatment easier
        for (int i = 4; i < 516; i++)
            *(dataToSend + i) = *(dataToSend + (512 * (block_counter - 1)) + i);

        // Updating the block_counter for the "dataToSend"
        dataToSend[2] = block_counter >> 8;
        dataToSend[3] = block_counter;

        if (block_counter >= blocksToSend) // Last 512 Bytes of "dataToSend"
        {
            // Move the remaining data to the buffer
            netbuf_ref(buf, dataToSend, (byte_counter % 512) + 4);
            // Send the buffer through the netconn "tftp_server"
            netconn_send(tftp_server, buf);

            // Disconnect the netconn "tftp_server" from its current peer
            netconn_disconnect(tftp_server);

            // The server can treat a new "RRQ"
            flag_client = 1;

        } else { // Before the last 512 Bytes of "dataToSend"
            // Move 516 Bytes of data to the buffer
            netbuf_ref(buf, dataToSend, 516);
            // Send the buffer through the netconn "tftp_server"
            netconn_send(tftp_server, buf);
        }
    }


}


