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



#include <vector>

#define ERROR_S "SERVER ERROR: "
#define DEFAULT_PORT 1601
#define BUFFER_SIZE 1024
#define CLIENT_CLOSE_CONNECTION_SYMBOL '#'



bool is_client_connection_close(const char* msg);

void howManyNumberInPacket(const char* msg, std::vector<int> &vecInt, int& sum);

void reverseVectorIntToString(std::vector<int> &vecInt, std::string& send);

void stringToChar(char *msg, const std::string& sendPacket);

void clearBuffer(char* buffer);

//void pushSumInSendPacket(const std::string sendPacket, int&sum);

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    int client;
    int server;

    struct sockaddr_in server_address;

    std::vector<int> vecInt;
    std::string sendPacket;
    int sum;

    client = socket(AF_INET, SOCK_STREAM, 0);//AF_INET протокол интернет IPv4
                                             //SOCK_STREAM сокет в формате tcp

    if(client < 0)
    {
        std::cout << "server error: establishing socket error.";
        std::cout << errno;
            return EXIT_FAILURE;
    }

    std::cout << "server: socket for server was successfull created" << std::endl;

    server_address.sin_port = htons(DEFAULT_PORT);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY);


    int ret = bind(client, reinterpret_cast<struct sockaddr*>(&server_address), sizeof(server_address));

    if(ret < 0)
    {
        std::cout << ERROR_S
            << "binding connection. socket has already benn establishing." << std::endl;
        return EXIT_FAILURE;
    }

    socklen_t size = sizeof(server_address);
    std::cout << "SERVER:" << " Listening client..." << std::endl;
    listen(client, 0);

    server = accept(client, reinterpret_cast<struct sockaddr*>(&server_address), &size);
    if(server < 0)
    {
        std::cout << ERROR_S << "Can't listening client" << std::endl;
    }

    char buffer[BUFFER_SIZE];
    bool isExit = false;
    while(server > 0)
    {
        strcpy(buffer, "=> Server connected!\n\n");
        send(server, buffer, BUFFER_SIZE, 0);


        while(!isExit)
        {

            std::cout << "Message from client: ";
            recv(server, buffer, BUFFER_SIZE, 0);
            std::cout << buffer << std::endl;


            howManyNumberInPacket(buffer, vecInt, sum);

            reverseVectorIntToString(vecInt, sendPacket);

            sendPacket.push_back('\n');
            sendPacket = sendPacket + std::to_string(sum);

            if(!vecInt.empty())
            {
                clearBuffer(buffer);
                stringToChar(buffer, sendPacket);
            }


            if(is_client_connection_close(buffer))
            {
                break;
            }


            std::cout << "Sending to client: " << buffer << std::endl;
            send(server, buffer, BUFFER_SIZE, 0);

            vecInt.clear();
            sendPacket.clear();


        }
        std::cout << "Goodbay..." << std::endl;
        isExit = false;
        exit(1);
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

void howManyNumberInPacket(const char* msg, std::vector<int>& vecInt, int &sum)
{
    std::string sumStr;
    bool thereNumber = false;
    sum = 0;
    int size = strlen(msg);
    int tmp;
    for(int i = 0; i < size; ++i)
    {
        if(msg[i] >= '0' && msg[i] <= '9')
        {
            sumStr.push_back(msg[i]);
            thereNumber = true;
        }
        else if(thereNumber)
        {   tmp = std::stoi(sumStr);
            sum += tmp;
            vecInt.push_back(tmp);
            sumStr.clear();
            thereNumber = false;
        }

    }

    if(!sumStr.empty())//если последней оказалась цифра
    {
        tmp = std::stoi(sumStr);
        sum += tmp;
        vecInt.push_back(tmp);
    }

}


void reverseVectorIntToString(std::vector<int> &vecInt, std::string &send)
{
    for(std::vector<int>::reverse_iterator itb = vecInt.rbegin(), ite = vecInt.rend(); itb != ite; ++itb)
    {
        send = send + std::to_string(*itb);
        if(itb != (ite -1))
        {
            send = send + " ";
        }
    }
}


void stringToChar(char* msg, const std::string& sendPacket)
{
    for(int i = 0; i < sendPacket.size(); i++)
    {
        msg[i] = sendPacket.at(i);
    }
}

void clearBuffer(char* buffer)
{
    for(int i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = '\0';
    }
}
