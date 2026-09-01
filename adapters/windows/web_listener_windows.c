/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/windows/web_listener_windows.c
 *
 * PURPOSE:
 *   Open and close the WinSock TCP listener for the Framework web server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The Windows adapter owns WinSock startup and socket details so they do not leak into the portable web API.
 */

#include "umicom/web/listener.h"
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
UmiStatus umi_web_listener_platform_open(const UmiWebServerConfig *config,UmiWebListener *out_listener){WSADATA data;SOCKET s;struct sockaddr_in addr;if(config==NULL||out_listener==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(WSAStartup(MAKEWORD(2,2),&data)!=0)return UMI_STATUS_IO_ERROR;s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);if(s==INVALID_SOCKET){WSACleanup();return UMI_STATUS_IO_ERROR;}addr.sin_family=AF_INET;addr.sin_port=htons(config->port);if(InetPtonA(AF_INET,config->bind_address,&addr.sin_addr)!=1||bind(s,(const struct sockaddr*)&addr,sizeof(addr))==SOCKET_ERROR||listen(s,SOMAXCONN)==SOCKET_ERROR){closesocket(s);WSACleanup();return UMI_STATUS_IO_ERROR;}out_listener->native_handle=(intptr_t)s;out_listener->open=1;out_listener->port=config->port;return UMI_STATUS_OK;}
void umi_web_listener_platform_close(UmiWebListener *listener){if(listener!=NULL&&listener->open){closesocket((SOCKET)listener->native_handle);listener->native_handle=-1;listener->open=0;WSACleanup();}}
#endif
