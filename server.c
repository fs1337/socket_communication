#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>

#define PORT 9999

//create a new output file
FILE* create_file()
{
    FILE *fp;
    fp = fopen("secret.txt", "w");
    if (fp == NULL) {
        printf("Sorry, the file can not open!\n");
        return NULL;
    }
    
    return fp;
}

int main()
{
    //create shared memory
    //id: the id for shared memory
    int key = 9999;
    int id;
    
    //create a new output file
    FILE *fp = create_file();
    
    //create socket
    int socket_ref;
    socket_ref = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_ref == -1) {
        printf("Failed to create socket!");
        exit(1);
    }
    
    //Identify the socket(binding it)
    //port number is 9999
    struct sockaddr_in serv_addr, client_addr;
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);
    if (bind(socket_ref, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Failed to bind socket!");
    }
    
    //listen for data
    while(1)
    {
        //printf("waiting\n");
        sleep(2);
        printf("Waiting for data: ");
        //fflush(stdout);
    
        //try to receive some data, this is a blocking call
        char line[128];
        socklen_t addrlen = sizeof(serv_addr);
        size_t recv_len;
        memset(line,'\0', 128);
        if ((recv_len = recvfrom(socket_ref, line, 128, 0, (struct sockaddr *)&client_addr, &addrlen)) == -1)
        {
            printf("recvfrom client error\n");
        }
        //print details of the client/peer and the data received
        printf("%s", line);
        int length = strlen(line);
        //this line has not read
        if(length > 0 && *line != '#')
        {
            fprintf(fp, "%d,%s",length-1, line);
            fflush(fp);
            //clear buffer
            memset(line,'\0', 128);
            //fill line with "#"
            *line = '#';
        }
        
        //send response to the client with "#"
        if (sendto(socket_ref, line, 1, 0, (struct sockaddr*) &client_addr, addrlen) == -1)
        {
            printf("send response to clent error!\n");
        }
        sleep(2);
    }
    
    //close socket
    close(socket_ref);
}
