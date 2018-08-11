#include <errno.h>
#include <arpa/inet.h>

#include "handlers.h"
#include "net_utils.h"
#include "servers_container.h"
#include <stdio.h>


/**
 * @brief This handler is used to define massage handler
 * @param sockfd - socket, that may be used for response send
 * @param buffer - recieved message buffer
 * @param buffersize - message size
 * @param recvfrom -  peer address
 * @return 0 if success, less 0 on error
 */
typedef int (*message_handler)(int /*sockfd*/, const void*/*buffer*/, size_t /*buffersize*/, const struct sockaddr_in */*recvfrom*/);



/**
 * @brief Handler for init message
 */
static int respond_for_init_messaage(int sockfd, const void *buf, size_t len, const struct sockaddr_in *src_addr);

/**
 * @brief Handler replay of init message
 */
static int descover_server(int sockfd, const void *buf, size_t len, const struct sockaddr_in *src_addr);

/**
 * @brief Prepare ping message
 */
static int prepare_notify_message(int sockfd, const void *buf, size_t len, const struct sockaddr_in *src_addr);


/**
  * Array of message handlers. Index - message type, value - handler
  */
static message_handler HANDLERS[MESSAGES_SIZE] = {
    respond_for_init_messaage, /*INIT_MESSAGE*/
    descover_server,           /*DESCOVER_MESSAGE*/
    prepare_notify_message     /*NOTIFY_MESSAGE*/
};

int prepare_message(int sockfd, const void *buf, size_t len, const struct sockaddr_in *src_addr, uint32_t identifier){
    int  message_type;
    const char *p = buf;
    uint32_t get_ident;

    if(len < sizeof (int))
    {
        printf("invalid buffer size. get %lu, expected %lu\n", len, sizeof (int));
        return INVALID_FORMAT_ERR;
    }

    message_type = *((int *)p);

    if(INIT_MESSAGE == message_type){
        p += sizeof (int);
        get_ident = *((uint32_t *)p);
        if(get_ident == identifier){
            printf("get message %d from local process %d %s\n", message_type, identifier, inet_ntoa(src_addr->sin_addr));
            return NO_ERR;
        }
    }

    if( message_type >= MESSAGES_SIZE || message_type < 0){
        printf("Invalid message type: %d\n", message_type);
        return INVALID_FORMAT_ERR;
    }

    return (HANDLERS[message_type])(sockfd, buf, len, src_addr);
}


int respond_for_init_messaage(int sockfd, const void *buf, size_t len, const struct sockaddr_in *src_addr){

    int mess = DESCOVER_MESSAGE, rc;

    UNUSED(buf);
    UNUSED(len);
    printf("Get init mesage from: %s\n", inet_ntoa(src_addr->sin_addr));


    if(sendto(sockfd, &mess, sizeof(mess), 0, (struct sockaddr *)src_addr, ADDR_LEN) < 0){
        printf("Failed send message as responze to init message %d\n", errno);
        return SEND_ERR;
    }

    if((rc = add_server(src_addr)) < 0){
        return rc;
    }

    return NO_ERR;
}

int descover_server(int sockfd, const void *buf, size_t len, const struct sockaddr_in *src_addr){
    printf("Get descover mesage from: %s\n", inet_ntoa(src_addr->sin_addr));
    UNUSED(sockfd);
    UNUSED(buf);
    UNUSED(len);
    return add_server(src_addr);
}

int prepare_notify_message(int sockfd, const void *buf, size_t len, const struct sockaddr_in *src_addr){
   printf("Get active mesage from: %s\n", inet_ntoa(src_addr->sin_addr));
   UNUSED(sockfd);
   UNUSED(buf);
   UNUSED(len);
   return server_acive(src_addr);
}
