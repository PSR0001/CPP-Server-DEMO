/*************************************************************
 * 
 *  compile command = "g++ main.cpp -o main -lws2_32" 
 * 
 * 
 * **********************************************************/
// #define _WIN32_WINNT
#define WINVER WindowsXP

#include <iostream>
#include<winsock2.h>
#include<winsock.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;


//function prototype

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);

int main()
{

    // initialize winsock
    WSADATA wsData;
    

    int wsOK = WSAStartup(MAKEWORD(2, 2), &wsData);
    if (wsOK != 0)
    {
        cerr << "Can't initilised the variable! Quiting" << endl;
        return 0;
    }

    // Creating a Socket
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening != INVALID_SOCKET)
    {
        cerr << "can't initilised the variable! Quiting" << endl;
        return 0;
    }

    // Bind the IP address and a PORT to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also used inet_pton ...

    bind(listening, (sockaddr *)&hint, sizeof(hint));

    // Tell winsock the socket is for listening
    listen(listening, SOMAXCONN);

    //wait for a connection
    sockaddr_in client;
    int clientSize = sizeof(client);

    SOCKET clientSocket = accept(listening,(sockaddr*)&client,&clientSize);

    char host[NI_MAXHOST]; //client's remote name
    char service[NI_MAXSERV]; // service (i.e port)the client is connected on
    
    ZeroMemory(host, NI_MAXHOST);
    ZeroMemory(service,NI_MAXSERV); 

    if (getnameinfo((sockaddr*)&client,sizeof(client),host,NI_MAXHOST,service,NI_MAXSERV,0)==0)
    {
       cout<< host << " Connected to PORT "<< service<<endl;
    }
    else{
        inet_ntop(AF_INET,&client.sin_addr,host,NI_MAXHOST);
        cout<< host << " Connected to PORT " << ntohs(client.sin_port)<<endl;

    }

    //Close the Listening Socket
    closesocket(listening);

    // While Loop: accept and echo the message to the client
    char buf[4096];

    while(true){
        ZeroMemory(buf , 4096);

        int bytesReceived = recv(clientSocket,buf,4096,0);
        if( bytesReceived == SOCKET_ERROR){
            cerr<< "Error in recv(). Quiting..."<<endl;
            break;
        }
        if( bytesReceived == 0){
            cerr<< "Client Disconnected. Quiting..."<<endl;
            break;
        }
    // echo the message 
    send(clientSocket,buf,bytesReceived+1,0);
    }

    //close the socket
    closesocket(clientSocket);

    // Cleanup winsocket
    WSACleanup();
}

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
  struct sockaddr_storage ss;
  unsigned long s = size;

  ZeroMemory(&ss, sizeof(ss));
  ss.ss_family = af;

  switch(af) {
    case AF_INET:
      ((struct sockaddr_in *)&ss)->sin_addr = *(struct in_addr *)src;
      break;
    case AF_INET6:
      ((struct sockaddr_in6 *)&ss)->sin6_addr = *(struct in6_addr *)src;
      break;
    default:
      return NULL;
  }
  /* cannot direclty use &size because of strict aliasing rules */
  return (WSAAddressToString((struct sockaddr *)&ss, sizeof(ss), NULL, dst, &s) == 0)?
          dst : NULL;
}