#include "net_utils.h"
#include <net/if.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>

static int get_ifconf(int sockfd, struct ifconf * const ifc);
static void log_fri(const struct ifreq *ifr);
static char* buff_add(char **buff, int *buff_len, const char *add);

int get_brodcast_addr(struct sockaddr_in * const addr)
{
    int sockfd, err, flags;
    struct ifconf ifc;
    char *ptr;
    struct ifreq *ifr, ifrcopy;
    int res = NOT_FOUND_ERR;


    if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0) ) < 0){
        return SOCKET_ERR;
    }

    if( (err = get_ifconf(sockfd, &ifc)) != NO_ERR ) return err;

    for(ptr = ifc.ifc_ifcu.ifcu_buf; ptr < ifc.ifc_ifcu.ifcu_buf + ifc.ifc_len;){
        ifr = (struct ifreq *)ptr;
        ptr += sizeof (struct ifreq);

        ifrcopy = *ifr;
        if(ioctl(sockfd, SIOCGIFFLAGS, &ifrcopy) < 0){
            res = IOCTL_ERR;
            break;
        }

        flags = ifrcopy.ifr_ifru.ifru_flags;


        log_fri(ifr);


        if(ifr->ifr_ifru.ifru_addr.sa_family == AF_INET && (flags & IFF_UP) && (flags & IFF_BROADCAST) ){
           ifrcopy = *ifr;
           if(ioctl(sockfd, SIOCGIFBRDADDR, &ifrcopy) < 0){
                res = IOCTL_ERR;
                break;
            }
           else{
                memcpy(addr, &(ifrcopy.ifr_ifru.ifru_broadaddr), sizeof (struct sockaddr_in));
                res = NO_ERR;
                break;
           }
        }
    }
    free(ifc.ifc_ifcu.ifcu_buf);
    return res;

}




int get_ifconf(int sockfd, struct ifconf * const ifc){
    char *buff;
    int lastlen = 0, len = 100 * sizeof (struct ifreq);

    for(;;){
        buff = malloc(len);
        if(NULL == buff) return NO_MEMORY_ERR;
        ifc->ifc_len = len;
        ifc->ifc_ifcu.ifcu_buf = buff;
        if(ioctl(sockfd, SIOCGIFCONF, ifc) < 0){
            if(errno != EINVAL || lastlen != 0){
                return IOCTL_ERR;
            }
        }else{
            if(ifc->ifc_len == lastlen) break;
            lastlen = ifc->ifc_len;
        }
        len += 10 * sizeof (struct ifreq);
        free(buff);
    }

    return NO_ERR;
}



void log_fri(const struct ifreq *ifr){
    char message[1000], *p = message, addr[100];
    int mess_len = sizeof (message);


    memset(message, 0, mess_len);
    memset(addr, 0, sizeof (addr));

    buff_add(&p, &mess_len, "Family: ");
    if(ifr->ifr_ifru.ifru_addr.sa_family == AF_INET){
       buff_add(&p, &mess_len, "IPv4");
    }else{
       buff_add(&p, &mess_len, "IPv6");
    }

    buff_add(&p, &mess_len, ", name: ");
    buff_add(&p, &mess_len, ifr->ifr_ifrn.ifrn_name);
    buff_add(&p, &mess_len, ", address: ");
    buff_add(&p, &mess_len, inet_ntoa(( (struct sockaddr_in *)&ifr->ifr_broadaddr )->sin_addr));

    printf("log_fri: %s\n", message);
}

static char* buff_add(char **buff, int *buff_len, const char *add){
   char *out = *buff;
   int add_len = 0;
   if(buff == NULL || add == NULL) return NULL;

   add_len =  strlen(add);

   if(*buff_len < add_len) return NULL;
   strncpy(out,add, add_len);
   *buff_len -= add_len;
   *buff = out + add_len;
   return (*buff);
}

int get_brodcast_addr_by_name(struct sockaddr_in * const addr, const char *name){
    int sockfd;
    struct ifreq ifr;

    memset(&ifr, 0, sizeof (struct ifreq));
    if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0) ) < 0){
        return SOCKET_ERR;
    }
     ifr.ifr_addr.sa_family = AF_INET;

    strncpy(ifr.ifr_ifrn.ifrn_name, name, sizeof (ifr.ifr_ifrn.ifrn_name));

    if(ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0 || !(ifr.ifr_ifru.ifru_flags & IFF_UP) || !(ifr.ifr_ifru.ifru_flags & IFF_BROADCAST) || ioctl(sockfd, SIOCGIFBRDADDR, &ifr) < 0){
        return NOT_FOUND_ERR;
    }else{
        memcpy(addr, &(ifr.ifr_ifru.ifru_broadaddr), sizeof (struct sockaddr_in));
        printf("%s - %s\n" , name , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_broadaddr )->sin_addr) );
        return  NO_ERR;
    }
}

uint32_t get_server_identifier(const char *name){
    int sockfd;
    struct ifreq ifr;
    struct sockaddr_in *addr;

    memset(&ifr, 0, sizeof (struct ifreq));
    if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0) ) < 0){
        return 0;
    }

    ifr.ifr_addr.sa_family = AF_INET;

    strncpy(ifr.ifr_ifrn.ifrn_name, name, sizeof (ifr.ifr_ifrn.ifrn_name));

    if(ioctl(sockfd, SIOCGIFADDR, &ifr) < 0){
        return 0;
    }else{
        if(ifr.ifr_ifru.ifru_addr.sa_family != AF_INET) return 0;

        addr = (struct sockaddr_in*)&ifr.ifr_ifru.ifru_addr;
        return (uint32_t) addr->sin_addr.s_addr;
    }
}
