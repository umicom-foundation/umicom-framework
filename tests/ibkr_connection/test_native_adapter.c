/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_native_adapter.c
 * PURPOSE:
 *   Exercise nonblocking native I/O on a private ephemeral loopback listener. Never use a
 *   configured provider port, account or broker protocol.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Exercise nonblocking native I/O on a private ephemeral loopback listener.
 * Never use a configured provider port, account or broker protocol. */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../../src/ibkr_connection/internal.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
typedef SOCKET TestSocket;
#define BAD INVALID_SOCKET
#define CloseSocket closesocket
_Static_assert(sizeof(long)==4U,"Winsock ABI long is 32 bits");
_Static_assert((u_long)(long)FIONBIO==(u_long)FIONBIO,"FIONBIO command word is unchanged");
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
typedef int TestSocket;
#define BAD (-1)
#define CloseSocket close
#endif
#define REQUIRE(x) do {if(!(x)){fprintf(stderr,"native adapter check line %d: %s\n",__LINE__,#x);goto failed;}}while(0)
int main(void)
{
    TestSocket listener=BAD, peer=BAD;
    UmiIbkrIo io={0};void (*destroy)(void *)=NULL;
#ifdef _WIN32
    WSADATA data;if(WSAStartup(MAKEWORD(2,2),&data)!=0)return 1;
#endif
    listener=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);REQUIRE(listener!=BAD);
    struct sockaddr_in address;memset(&address,0,sizeof address);address.sin_family=AF_INET;
    address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);address.sin_port=0;
    REQUIRE(bind(listener,(struct sockaddr*)&address,(int)sizeof address)==0);REQUIRE(listen(listener,1)==0);
#ifdef _WIN32
    int length=(int)sizeof address;
#else
    socklen_t length=sizeof address;
#endif
    REQUIRE(getsockname(listener,(struct sockaddr*)&address,&length)==0);
    REQUIRE(UmiIbkrNativeIo(&io,&destroy)==UMI_STATUS_OK);
    UmiStatus status=io.Open(io.context,ntohs(address.sin_port));
    REQUIRE(status==UMI_STATUS_OK||status==UMI_STATUS_BUSY);
    uint64_t deadline=UmiIbkrMonotonicMilliseconds()+3000U;
    while(status==UMI_STATUS_BUSY && UmiIbkrMonotonicMilliseconds()<deadline)status=io.Ready(io.context);
    REQUIRE(status==UMI_STATUS_OK);
    peer=accept(listener,NULL,NULL);REQUIRE(peer!=BAD);
    char buffer[8]={0};size_t count=999U;
    /* No pending bytes must return BUSY promptly instead of blocking. */
    REQUIRE(io.Read(io.context,buffer,sizeof buffer,&count)==UMI_STATUS_BUSY&&count==0U);
    REQUIRE(send(peer,"test",4,0)==4);
    do {status=io.Read(io.context,buffer,sizeof buffer,&count);}while(status==UMI_STATUS_BUSY && UmiIbkrMonotonicMilliseconds()<deadline);
    REQUIRE(status==UMI_STATUS_OK&&count==4U&&!memcmp(buffer,"test",4U));
    REQUIRE(io.Write(io.context,"ok",2U,&count)==UMI_STATUS_OK && count==2U);
    REQUIRE(recv(peer,buffer,2,0)==2 && !memcmp(buffer,"ok",2U));
    io.Close(io.context);io.Close(io.context);destroy(io.context);destroy=NULL;
    (void)CloseSocket(peer);(void)CloseSocket(listener);
#ifdef _WIN32
    (void)WSACleanup();
#endif
    puts("Native nonblocking loopback adapter passed. No broker was contacted.");return 0;
failed:
    if(destroy)destroy(io.context);
    if(peer!=BAD)(void)CloseSocket(peer);
    if(listener!=BAD)(void)CloseSocket(listener);
#ifdef _WIN32
    (void)WSACleanup();
#endif
    return 1;
}
