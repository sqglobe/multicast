#include "handlers.h"
#include "net_utils.h"
#include "servers_container.h"
#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <sys/select.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

struct alive_message_data{
  int sockfd;
  uint32_t identifier;
};


static const int NOTIFY_COFF = 3, INACTIVE_COFF = 6;

/**
 * @brief Creates and setups UDP socket, that listen defined port
 * @param port listen port
 * @return return created socket or error code (error code less than 0)
 */
static int init_socket(int port);

/**
 * @brief Sends multicast message, that means new server started
 * @param sockfd - soсket, that is used for the multicast rate
 * @param interfacem - name of interface, this name is used to exctract multicast address
 * @param port - destination port, equalt to server port
 * @param identifier - identifier of this server exemplar. Used to сutt of local messages
 * @return 0 on success, less that 0 on error
 */
static int send_init_message(int sockfd, const char * interfacem, int port, uint32_t identifier);

/**
 * @brief Notifies defined address, that this server is still alive. Used point-to-point connection.
 * @param addr - peer address, that will be notified
 * @param data - additional data, in this case - socket and server identifier (struct alive_message_data)
 */
static void notify_alive_message(const struct sockaddr_in *addr, void *data);

int init_socket(int port){
    const int on = 1;
    struct sockaddr_in servaddr;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sockfd < 0) return SOCKET_ERR;

    bzero(&servaddr, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = port;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(0 > bind(sockfd, (struct sockaddr *)&servaddr, ADDR_LEN)){
        printf("Bind failed: %d\n", errno);
        return SOCKET_ERR;
    }

    if(0 > setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof (on))){
        printf("Set Sock opt failed: %d\n", errno);
        return SOCKET_ERR;
    }

    return sockfd;
}

int start_server(const char * interfacem, int port, long wait){
    int sockfd, n, rc, adr_size =  ADDR_LEN, max_fd;
    char buffer[2000];
    struct sockaddr_in recvaddr;
    fd_set rset;
    struct timeval wait_time;
    uint32_t identifier;
    long int now, last_activity = (long int) time(0);
    struct alive_message_data alive_data;

    if((identifier = get_server_identifier(interfacem)) == 0) {
        printf("Failed get interfase identifier for %s\n", interfacem);
        return IOCTL_ERR;
    }

    if((sockfd = init_socket(port)) < 0) return sockfd;

    if( (rc = send_init_message(sockfd, interfacem, port, identifier) ) < 0) return rc;

    FD_ZERO(&rset);
    max_fd = sockfd + 1;

    printf("Start man circle\n");


    alive_data.sockfd = sockfd;
    alive_data.identifier = identifier;

    while(1){
        FD_SET(sockfd, &rset);
        wait_time.tv_sec = wait;
        wait_time.tv_usec = 0;

        if(( rc = select(max_fd, &rset, 0, 0, &wait_time)) < 0 ){
            printf("Failed recieve message: %d\n", errno);
            return RECIEVE_ERR;
        }

        if(FD_ISSET(sockfd, &rset)){
            n = recvfrom(sockfd, buffer, sizeof (buffer), 0, (struct sockaddr*)&recvaddr, (socklen_t *)&adr_size);
            if(n < 0){
                printf("Failed recieve message: %d\n", errno);
                return RECIEVE_ERR;
            }

            recvaddr.sin_port = port;

            if((rc = prepare_message(sockfd, buffer, n, &recvaddr, identifier)) < 0){
               printf("Failed prepare message %d\n", rc);
               return RECIEVE_ERR;
            }
        }

        now = (long int) time(0);

        if( (now - last_activity) > (NOTIFY_COFF * wait)){

            printf("Notify\n");

            if( (rc = cleare_inactive(wait * INACTIVE_COFF)) < 0){
                return rc;
            }

            iterate_servers_with_data(notify_alive_message, &alive_data);
            last_activity = now;
        }

    }

    return NO_ERR;
}

int send_init_message(int sockfd, const char * interfacem, int port, uint32_t identifier){
    struct sockaddr_in broadcast;    
    int init_message = INIT_MESSAGE;
    int rc;

    char buff[sizeof (uint32_t) + sizeof (int)], *p = buff;

    if( (rc = get_brodcast_addr_by_name(&broadcast, interfacem)) < 0 ) return rc;

    broadcast.sin_port = port;
    memset(buff, 0, sizeof (buff));

    memcpy(p, &init_message, sizeof(int));
    memcpy(p + sizeof (int),  &identifier, sizeof(uint32_t));

    /*
    *((int*)p) = INIT_MESSAGE;
    p += ;
    *((uint32_t*)p) = identifier;
    */

    if(0 > sendto(sockfd, buff, sizeof (buff), 0, (struct sockaddr*)&broadcast, ADDR_LEN)) {
        printf("Failed set init message: %d\n", errno);
        return SEND_ERR;
    }
    return NO_ERR;
}


void notify_alive_message(const struct sockaddr_in *addr, void *data){

    const struct alive_message_data * par = data;
    char buff[sizeof (int) + sizeof (uint32_t)], *p = buff;

    memset(buff, 0, sizeof (buff));

    *((int*)p) = NOTIFY_MESSAGE;
    p += sizeof (int);
    *((uint32_t*)p) = par->identifier;

    if(0 > sendto(par->sockfd, buff, sizeof (buff), 0, (struct sockaddr*)addr, sizeof (struct sockaddr_in))) {
        printf("Failed notify message %d to %s, port: %d\n", errno, inet_ntoa(addr->sin_addr), addr->sin_port);
    }

    printf("Notify server %s about alive\n", inet_ntoa(addr->sin_addr));
}
