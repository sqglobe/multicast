#ifndef RETURN_CODES_H
#define RETURN_CODES_H
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
#endif // RETURN_CODES_H
