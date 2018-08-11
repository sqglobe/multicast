#ifndef HANDLERS_H
#define HANDLERS_H
#include <netinet/ip.h> /* superset of previous */

enum MESSAGE_TYPES{
   INIT_MESSAGE = 0, /* Notifys other servers, that the new exeplar now available */
   DESCOVER_MESSAGE, /* Replay for the init message. Reports to the started server, that current exemplar is active */
   NOTIFY_MESSAGE, /* Ping message, notified remote servers that current exemplar is active*/
   MESSAGES_SIZE
};

/**
 * @brief This method is entry point for received message handling. It directs the messages depends on their type
 * @param sockfd - socket, that may be used for response send
 * @param buf - recieved message buffer
 * @param len - message size
 * @param src_addr -  peer address
 * @param identifier - peer identifier
 * @return 0 if success, less 0 on error
 */
int prepare_message(int sockfd, const void *buf, size_t len, const struct sockaddr_in *src_addr, uint32_t identifier);

#endif //HANDLERS_H
