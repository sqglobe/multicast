#include <stdio.h>
#include "server.h"

static const int PORT = 8900;

int main(int argc, const char* argv[] )
{
    int rc = 0;
    if(argc < 2){
        printf("arguments count < 2\n");
        return 2;
    }


    if( (rc = start_server(argv[1], PORT, 5))  < 0){
        printf("failed start server %d\n", rc);
    }

    return 0;
}
