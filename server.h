#ifndef SERVER_H
#define SERVER_H
#include <netinet/ip.h> /* superset of previous */

/**
 * @brief This method starts the server exemplar, that sends and receives messages
 * @param interfacem name of interface, that is used for multicast recieve
 * @param port audible server port
 * @param wait waits seconds input messages
 * @return 0 if server finished without error, less than 0 if server finished with error, or cannot be started
 */
int start_server(const char * interfacem, int port, long wait);


#endif //SERVER_H
