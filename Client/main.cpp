#include <QCoreApplication>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <cstring>


#define SERVER_IP "127.0.0.1"
#define ERROR_S "CLIENT ERROR: "
#define DEFAULT_PORT 1601
#define BUFFER_SIZE 1024
#define CLIENT_CLOSE_CONNECTION_SYMBOL '#'

bool is_client_connection_close(const char* msg);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int client;

    struct sockaddr_in server_address;

    client = socket(AF_INET, SOCK_STREAM, 0);

    if(client < 0)
    {
        std::cout << ERROR_S << "establishing socket error." << std::endl;
            return EXIT_FAILURE;
    }

    server_address.sin_port = htons(DEFAULT_PORT);
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP,&server_address.sin_addr);

    std::cout << "Clinet socket created" << std::endl;

    int ret = connect(client, reinterpret_cast<struct sockaddr*>(&server_address), sizeof(server_address));

    if(!ret)
    {
        std::cout << "COnnetction to server "
                  << inet_ntoa(server_address.sin_addr)
                  << " with portnumber " << DEFAULT_PORT << std::endl;
    }

    std::cout << "Waitinig for server connection " << std::endl;

    char buffer[BUFFER_SIZE];
    recv(client, buffer, BUFFER_SIZE,0);
    std::cout << "Connection established!" << std::endl
              << buffer << std::endl
              << "Enter " << CLIENT_CLOSE_CONNECTION_SYMBOL
              << " for close the connection"
              << std::endl;
    bool isExit = false;
    while (!isExit)
    {
        std::cout << "Client:" << std::endl;
        std::cin.getline(buffer, BUFFER_SIZE);
        send(client, buffer, BUFFER_SIZE, 0);


        if(is_client_connection_close(buffer))
        {
           break;//isExit = true;
        }

        std::cout << "Server:" << std::endl;
        recv(client, buffer, BUFFER_SIZE, 0);
        std::cout << buffer;

        if(is_client_connection_close(buffer))
        {
            break;//isExit = true;
        }

        std::cout << std::endl;

    }

    return a.exec();
}


bool is_client_connection_close(const char* msg)
{
    for(unsigned int i = 0; i < strlen(msg); ++i)
    {
        if(msg[i] == CLIENT_CLOSE_CONNECTION_SYMBOL)
        {
            return true;
        }
    }
    return false;
}
