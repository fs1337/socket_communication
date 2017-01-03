#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>

#define PORT 9999
#define SIZE 128

char* receive(char *line, const char *code)
{
    if(strstr(line, code) != NULL)
    {
        return line;
    }
    else
    {
        return NULL;
    }
}

FILE* read_file(char *p)
{
    //check whether the file exists or not
    if(p == NULL)
    {
        return NULL;
    }
    
    //open file
    FILE *fp;
    fp = fopen(p, "r");
    if (fp == NULL) {
        printf("Sorry, the file can not open!\n");
        return NULL;
    }
    
    return fp;
}

int main(int argc, char **argv)
{
    int socket_ref;
    
    //create socket
    socket_ref = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_ref == -1) {
        printf("Failed to create socket!");
        exit(1);
    }

    //set server's address
    struct sockaddr_in serv_addr;
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);
    //get server's ip address from arguments
    char *server = argv[1];
    if (inet_aton(server , &serv_addr.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    
    //if the line contains the secret code "C00L".
    //sends this information to the the shared memory
    while(1)
    {
        //get alpha numeric strings as input from the user
        //one line at a time
        sleep(2);
        
        //read file
        char line1[128];
        char *line = line1;
        printf("Please input your command:\n");
        size_t len = 128;
        int flag;
        if((flag = getline(&line, &len, stdin)) == -1)
        {
            return 0;
        }
        else
        {
            printf ("line is %s", line1);
            sleep(1);
            if(receive(line, "C00L")!= NULL)
            {
                //send data to other machine, for udp type data
                //send the message
                if (sendto(socket_ref, line, strlen(line), 0 , (struct sockaddr *)&serv_addr, sizeof(serv_addr))==-1)
                {
                    printf("send data to server error!\n");
                }
                //clear buffer
                memset(line,'\0', 128);
                //receive data from other machine
                socklen_t addrlen = sizeof(serv_addr);
                if (recvfrom(socket_ref, line, 128, 0, (struct sockaddr *)&serv_addr, &addrlen) == -1)
                {
                    printf("get response error!");
                }
                //wait for response
                while(*line != '#')
                {
                    sleep(1);
                }
            }

        }
    }
    close(socket_ref);
}
