/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/posix/web_listener_posix.c
 *
 * PURPOSE:
 *   Open and close the POSIX TCP listener for the Framework web server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Only this adapter uses POSIX socket APIs. The rest of the Framework talks through UmiWebListener.
 */

#include "umicom/web/listener.h"
#if !defined(_WIN32)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
UmiStatus umi_web_listener_platform_open(const UmiWebServerConfig *config,UmiWebListener *out_listener){int fd,opt=1;struct sockaddr_in addr;if(config==NULL||out_listener==NULL)return UMI_STATUS_INVALID_ARGUMENT;fd=socket(AF_INET,SOCK_STREAM,0);if(fd<0)return UMI_STATUS_IO_ERROR;(void)setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));(void)memset(&addr,0,sizeof(addr));addr.sin_family=AF_INET;addr.sin_port=htons(config->port);if(inet_pton(AF_INET,config->bind_address,&addr.sin_addr)!=1){(void)close(fd);return UMI_STATUS_INVALID_ARGUMENT;}if(bind(fd,(struct sockaddr*)&addr,sizeof(addr))!=0||listen(fd,16)!=0){(void)close(fd);return UMI_STATUS_IO_ERROR;}out_listener->native_handle=(intptr_t)fd;out_listener->open=1;out_listener->port=config->port;return UMI_STATUS_OK;}
void umi_web_listener_platform_close(UmiWebListener *listener){if(listener!=NULL&&listener->open){(void)close((int)listener->native_handle);listener->native_handle=-1;listener->open=0;}}
#endif
