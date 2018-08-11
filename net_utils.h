#ifndef NET_UTILS_H
#define NET_UTILS_H
#include <netinet/ip.h> /* superset of previous */


#define UNUSED(x) (void)(x)

#define ADDR_LEN sizeof(struct sockaddr_in)

enum ERRORS{
             NO_ERR = 0,
             NO_MEMORY_ERR = -1,
             IOCTL_ERR = -2,
             SOCKET_ERR = -3,
             NOT_FOUND_ERR = -4,
             UNKNOWN_ERR = -5,
             RECIEVE_ERR = -6,
             SEND_ERR = -7,
             INVALID_FORMAT_ERR = -8
           };


/**
 * @brief Retrieves first available broadcast address
 * @param addr - used to write the broadcast address
 * @return  0 if success, less  that 0 on error
 */
int get_brodcast_addr(struct sockaddr_in * const addr);

/**
 * @brief Return server identifier for used interface name
 * @param name - used interface name
 * @return 0 on error or server identifier on success
 */
uint32_t get_server_identifier(const char *name);

/**
 * @brief Retrieves broadcast address for the specified name of the interface
 * @param addr - used to write extracted address
 * @param name - interface name, that is used for broadcast rate
 * @return 0 on success, less 0 if error occure
 */
int get_brodcast_addr_by_name(struct sockaddr_in * const addr, const char *name);

#endif //NET_UTILS_H
