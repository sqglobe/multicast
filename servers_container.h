#ifndef SERVERS_CONTAINER_H
#define SERVERS_CONTAINER_H
#include <netinet/ip.h> /* superset of previous */

/**
 * Container store the addresses of discovered servers
 */

/**
 * Handler that used to iterate over container. Takes server address
 */
typedef void (*for_each)(const struct sockaddr_in *);

/**
 * Handler that used to iterate over container. Takes server address and additional data
 */
typedef void (*for_each_data)(const struct sockaddr_in *, void*);


/**
 * @brief Adds new server to the container
 * @param src_addr - server address
 * @return 0 on success, less than 0 if error occured
 */
int add_server(const struct sockaddr_in *src_addr);

/**
 * @brief Removes server from the container
 * @param src_addr - server address
 * @return 0 on success, less than 0 if error occured
 */
int remove_server(const struct sockaddr_in *src_addr);

/**
 * @brief Marks the server as active
 * @param src_addr - server address
 * @return 0 on success, less than 0 if error occured
 */
int server_acive(const struct sockaddr_in *src_addr);

/**
 * @brief Removes servers, that are inactive more than specified count of seconds
 * @param active_time - watershed of inactivity in seconds. If server had been marked as active more than specified seconds count, that server will be removed
 * @return 0 on success, less than 0 if error occured
 */
int cleare_inactive(long active_time);

/**
 * @brief Detects that specified server contains in container.
 * @param src_addr - server address
 * @return 1 is server contained yet,  0 otherwise
 */
int has_server(const struct sockaddr_in *src_addr);

/**
 * @brief Iterates over container used specified handler. Handler will get only server address.
 * @param handler - handler, that will be called for each server in container
 */
void iterate_servers(for_each *handler);

/**
 * @brief Iterate over container and call handler for each server. Handler will get server address and additional data. This data takes accross this method
 * @param handler - handler, that will be called for each server in container
 * @param data - additional data for handler
 */
void iterate_servers_with_data(for_each_data handler, void *data);


#endif //SERVERS_CONTAINER_H
