#include "servers_container.h"
#include "net_utils.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

struct server_item{
  struct sockaddr_in *address;
  time_t last_activity;
  struct  server_item *next;
};


static struct server_item *HEADER = NULL;


static struct server_item *make_item(const struct sockaddr_in *src_addr);
static void free_item(struct server_item * item);


struct server_item *make_item(const struct sockaddr_in *src_addr){
    struct server_item *item = malloc(sizeof (struct server_item));

    if(NULL == item) return NULL;
    item->address = malloc(sizeof (struct sockaddr_in));

    if(NULL == item->address){
        free(item);
        return NULL;
    }

    memcpy(item->address, src_addr, sizeof (struct sockaddr_in));
    item->last_activity = time(NULL);
    item->next = NULL;

    return item;
}


void free_item(struct server_item * item){
    free(item->address);
    free(item);
    item = NULL;
}



int add_server(const struct sockaddr_in *src_addr){
    struct server_item *item;
    if(!has_server(src_addr)){
        if(NULL == (item = make_item(src_addr))) return NO_MEMORY_ERR;

        item->next = HEADER;
        HEADER = item;
    }
    return NO_ERR;
}

int remove_server(const struct sockaddr_in *src_addr){
    struct server_item *item = HEADER, *prev = NULL;

    while(item != NULL){
        if(0 == memcmp(src_addr, item->address, sizeof (struct sockaddr_in))){
            if(NULL == prev){
                HEADER = item->next;
                free_item(item);
            }
            else{
                prev->next = item->next;
                free_item(item);
            }
            return NO_ERR;
        }
        prev = item;
        item = item->next;
    }

    return NOT_FOUND_ERR;
}

int server_acive(const struct sockaddr_in *src_addr){
    struct server_item *item;

    for(item = HEADER; item != NULL; item = item->next){
        if(0 == memcmp(src_addr, item->address, ADDR_LEN) ){
            item->last_activity = time(NULL);
            return NO_ERR;
        }
    }
    return add_server(src_addr);
}

int has_server(const struct sockaddr_in *src_addr){
    struct server_item *item;
    for(item = HEADER; item != NULL; item = item->next){
        if(0 == memcmp(src_addr, item->address, ADDR_LEN)){
            return 1;
        }
    }
    return 0;
}

int cleare_inactive(long active_time){
    long int now_tm = (long int) time(NULL), last_tm;
    struct server_item *item = HEADER, *prev = NULL;
    int count = 0;

    while(item != NULL){
        last_tm = (long int)item->last_activity;
        if((now_tm - last_tm) > active_time){
            if(NULL == prev){
                HEADER = item->next;
                free_item(item);
            }
            else{
                prev->next = item->next;
                free_item(item);
            }

            count += 1;
        }
        prev = item;
        item = item->next;
    }

    return count;
}

void iterate_servers(for_each *handler){
    struct server_item *item;
    for(item = HEADER; item != NULL; item = item->next){
        (*handler)(item->address);
    }
}

void iterate_servers_with_data(for_each_data handler, void *data){
    struct server_item *item;
    for(item = HEADER; item != NULL; item = item->next){
        (*handler)(item->address, data);
    }
}
