/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Loopback-only, nonblocking socket adapter. No DNS, credentials, shell or SDK.
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "internal.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
typedef SOCKET UmiSocket;
#define UMI_BAD_SOCKET INVALID_SOCKET
#else
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
typedef int UmiSocket;
#define UMI_BAD_SOCKET (-1)
#endif
typedef struct UmiLocalSocket { UmiSocket socket; bool startup; } UmiLocalSocket;
static bool WouldBlock(void)
{
#ifdef _WIN32
    int e=WSAGetLastError();return e==WSAEWOULDBLOCK||e==WSAEINPROGRESS||e==WSAEINTR;
#else
    return errno==EAGAIN||errno==EWOULDBLOCK||errno==EINPROGRESS||errno==EINTR;
#endif
}
static void Close(void *context)
{
    UmiLocalSocket *s=context;
    if(s->socket!=UMI_BAD_SOCKET){
#ifdef _WIN32
        (void)closesocket(s->socket);
#else
        (void)close(s->socket);
#endif
        s->socket=UMI_BAD_SOCKET;
    }
#ifdef _WIN32
    if(s->startup){(void)WSACleanup();s->startup=false;}
#endif
}
static UmiStatus Open(void *context,uint16_t port)
{
    UmiLocalSocket *s=context;
#ifdef _WIN32
    WSADATA data;if(WSAStartup(MAKEWORD(2,2),&data))return UMI_STATUS_IO_ERROR;s->startup=true;
    s->socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(s->socket==UMI_BAD_SOCKET)return UMI_STATUS_IO_ERROR;
    u_long nonblocking=1;
    /* Winsock declares cmd as signed long, while MinGW defines FIONBIO as an
     * unsigned command word. Preserve that word explicitly at the API boundary;
     * this is not a negative byte count or a change to nonblocking behaviour.
     * The previous implicit conversion remains below for engineering review. */
#if 0
    if(ioctlsocket(s->socket,FIONBIO,&nonblocking)!=0||!SetHandleInformation((HANDLE)s->socket,HANDLE_FLAG_INHERIT,0))return UMI_STATUS_IO_ERROR;
#endif
    _Static_assert(sizeof(long) == sizeof(u_long), "Winsock command widths must agree");
    _Static_assert((u_long)(long)FIONBIO == (u_long)FIONBIO,
                   "Winsock nonblocking command bits must survive the conversion");
    if(ioctlsocket(s->socket,(long)FIONBIO,&nonblocking)!=0||!SetHandleInformation((HANDLE)s->socket,HANDLE_FLAG_INHERIT,0))return UMI_STATUS_IO_ERROR;
#else
    s->socket=socket(AF_INET,SOCK_STREAM,0);
    if(s->socket==UMI_BAD_SOCKET)return UMI_STATUS_IO_ERROR;
    int flags=fcntl(s->socket,F_GETFL,0);if(flags<0||fcntl(s->socket,F_SETFL,flags|O_NONBLOCK)<0||fcntl(s->socket,F_SETFD,FD_CLOEXEC)<0)return UMI_STATUS_IO_ERROR;
    if(s->socket>=FD_SETSIZE)return UMI_STATUS_CAPACITY_EXCEEDED;
#if !defined(MSG_NOSIGNAL) && defined(SO_NOSIGPIPE)
    int noSignal=1;
    if(setsockopt(s->socket,SOL_SOCKET,SO_NOSIGPIPE,&noSignal,sizeof noSignal)!=0)return UMI_STATUS_IO_ERROR;
#elif !defined(MSG_NOSIGNAL)
    return UMI_STATUS_UNAVAILABLE; /* Never change the application's global signal policy. */
#endif
#endif
    struct sockaddr_in address;memset(&address,0,sizeof address);
    address.sin_family=AF_INET;address.sin_port=htons(port);address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    int result=connect(s->socket,(struct sockaddr *)&address,(int)sizeof address);
    if(result==0)return UMI_STATUS_OK;
    return WouldBlock()?UMI_STATUS_BUSY:UMI_STATUS_IO_ERROR;
}
static UmiStatus Ready(void *context)
{
    UmiLocalSocket *s=context;
    fd_set writeSet,errorSet;FD_ZERO(&writeSet);FD_ZERO(&errorSet);FD_SET(s->socket,&writeSet);FD_SET(s->socket,&errorSet);
    struct timeval timeout={0,0};
#ifdef _WIN32
    int result=select(0,NULL,&writeSet,&errorSet,&timeout);
#else
    int result=select(s->socket+1,NULL,&writeSet,&errorSet,&timeout);
#endif
    if(result<0)return WouldBlock()?UMI_STATUS_BUSY:UMI_STATUS_IO_ERROR;
    if(result==0)return UMI_STATUS_BUSY;
    int error=0;
#ifdef _WIN32
    int length=(int)sizeof error;if(getsockopt(s->socket,SOL_SOCKET,SO_ERROR,(char *)&error,&length))return UMI_STATUS_IO_ERROR;
#else
    socklen_t length=sizeof error;if(getsockopt(s->socket,SOL_SOCKET,SO_ERROR,&error,&length))return UMI_STATUS_IO_ERROR;
#endif
    return error==0?UMI_STATUS_OK:UMI_STATUS_IO_ERROR;
}
static UmiStatus Read(void *context,void *buffer,size_t capacity,size_t *out)
{
    UmiLocalSocket *s=context;*out=0;
    if(!capacity||capacity>(size_t)INT_MAX)return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    int count=recv(s->socket,buffer,(int)capacity,0);
#else
    ssize_t count=recv(s->socket,buffer,capacity,0);
#endif
    if(count<0)return WouldBlock()?UMI_STATUS_BUSY:UMI_STATUS_IO_ERROR;
    *out=(size_t)count;return UMI_STATUS_OK;
}
static UmiStatus Write(void *context,const void *buffer,size_t count,size_t *out)
{
    UmiLocalSocket *s=context;*out=0;
    if(!count||count>(size_t)INT_MAX)return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    int sent=send(s->socket,buffer,(int)count,0);
#else
#ifdef MSG_NOSIGNAL
    ssize_t sent=send(s->socket,buffer,count,MSG_NOSIGNAL);
#else
    ssize_t sent=send(s->socket,buffer,count,0);
#endif
#endif
    if(sent<0)return WouldBlock()?UMI_STATUS_BUSY:UMI_STATUS_IO_ERROR;
    *out=(size_t)sent;return UMI_STATUS_OK;
}
static void Destroy(void *context){Close(context);free(context);}
UmiStatus UmiIbkrNativeIo(UmiIbkrIo *out,void (**destroy)(void *))
{
    if(!out||!destroy)return UMI_STATUS_INVALID_ARGUMENT;
    UmiLocalSocket *s=calloc(1,sizeof *s);if(!s)return UMI_STATUS_OUT_OF_MEMORY;
    s->socket=UMI_BAD_SOCKET;
    *out=(UmiIbkrIo){s,Open,Ready,Read,Write,Close};*destroy=Destroy;return UMI_STATUS_OK;
}
uint64_t UmiIbkrMonotonicMilliseconds(void)
{
#ifdef _WIN32
    return (uint64_t)GetTickCount64();
#else
    struct timespec t;if(clock_gettime(CLOCK_MONOTONIC,&t)!=0)return 0;
    return (uint64_t)t.tv_sec*1000U+(uint64_t)t.tv_nsec/1000000U;
#endif
}
