/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/sdk_sse_reader/main.c
 *
 * PURPOSE:
 *   Read authored SSE fragments through an installed Framework SDK and preserve coalesced messages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/sse_parser.h"
#include "umicom/web/sse_response.h"
#include <stdio.h>
#include <string.h>
/* Extend the installed Web SDK example with response admission before payload decoding while retaining the original event-only walkthrough. The former implementation is retained for owner review. */
#if 0
int main(void)
{
    /* These fragments model transport boundaries only. No connection, model
     * inference, download or background worker is started by this example. */
    static const char *chunks[]={
        "\xef", "\xbb\xbf""event: update\r\nid: 7\r\ndata: Native \xe2",
        "\x82\xac\r\n\r\ndata: Second event\r\n\r\n"
    };
    UmiWebSseLimits limits={4096,65536,128,128,1048576};
    UmiWebSseParser *parser=NULL;
    UmiStatus status=UmiWebSseParserCreate(&limits,&parser);
    if(status!=UMI_STATUS_OK)goto failed;
    for(size_t i=0U;i<sizeof chunks/sizeof chunks[0];++i){
        size_t offset=0U,length=strlen(chunks[i]);
        while(offset<length){
            size_t consumed=0U;
            status=UmiWebSseParserFeed(parser,chunks[i]+offset,length-offset,&consumed);
            offset+=consumed;
            if(status==UMI_STATUS_OK){
                UmiWebSseMessage *message=NULL;
                status=UmiWebSseParserTake(parser,&message);
                if(status!=UMI_STATUS_OK)goto failed;
                int result=printf("Authored event %s, id %s: %s\n",
                    message->event,message->id,message->data);
                UmiWebSseMessageFree(message);
                if(result<0){status=UMI_STATUS_IO_ERROR;goto failed;}
            }else if(status!=UMI_STATUS_BUSY)goto failed;
        }
    }
    status=UmiWebSseParserFinish(parser);
    if(status!=UMI_STATUS_OK)goto failed;
    UmiWebSseParserDestroy(parser);
    if(fflush(stdout)!=0 || ferror(stdout))return 3;
    return 0;
failed:
    fprintf(stderr,"SSE reader failed: %s\n",umi_status_text(status));
    UmiWebSseParserDestroy(parser);
    return 2;
}
#endif
int main(void)
{
    /* These fragments model transport boundaries only. No connection, model
     * inference, download or background worker is started by this example. */
    static const char *chunks[]={
        "\xef", "\xbb\xbf""event: update\r\nid: 7\r\ndata: Native \xe2",
        "\x82\xac\r\n\r\ndata: Second event\r\n\r\n"
    };
    UmiWebSseLimits limits={4096,65536,128,128,1048576};
    UmiWebSseParser *parser=NULL;
    UmiWebSseResponse *response=NULL;
    UmiStatus status=UmiWebSseParserCreate(&limits,&parser);
    if(status!=UMI_STATUS_OK)goto failed;
    /* Header callbacks supply complete lines; payload callbacks can split at
     * any byte. This fixture exercises the same separation without HTTP. */
    UmiWebSseResponseLimits response_limits={4096U,32768U,128U,4U,1048576U};
    status=UmiWebSseResponseCreate(&response_limits,&response);
    if(status!=UMI_STATUS_OK)goto failed;
    static const char *headers[]={"HTTP/1.1 100 Continue\r\n","\r\n",
        "HTTP/1.1 200 OK\r\n","Content-Type: text/event-stream; charset=utf-8\r\n","\r\n"};
    for(size_t i=0U;i<sizeof headers/sizeof headers[0];++i){
        status=UmiWebSseResponseHeader(response,headers[i],strlen(headers[i]));
        if(status!=UMI_STATUS_OK)goto failed;
    }
    for(size_t i=0U;i<sizeof chunks/sizeof chunks[0];++i){
        size_t offset=0U,length=strlen(chunks[i]);
        status=UmiWebSseResponseBody(response,length);
        if(status!=UMI_STATUS_OK)goto failed;
        while(offset<length){
            size_t consumed=0U;
            status=UmiWebSseParserFeed(parser,chunks[i]+offset,length-offset,&consumed);
            offset+=consumed;
            if(status==UMI_STATUS_OK){
                UmiWebSseMessage *message=NULL;
                status=UmiWebSseParserTake(parser,&message);
                if(status!=UMI_STATUS_OK)goto failed;
                int result=printf("Authored event %s, id %s: %s\n",
                    message->event,message->id,message->data);
                UmiWebSseMessageFree(message);
                if(result<0){status=UMI_STATUS_IO_ERROR;goto failed;}
            }else if(status!=UMI_STATUS_BUSY)goto failed;
        }
    }
    status=UmiWebSseResponseFinish(response);
    if(status!=UMI_STATUS_OK)goto failed;
    status=UmiWebSseParserFinish(parser);
    if(status!=UMI_STATUS_OK)goto failed;
    UmiWebSseParserDestroy(parser);
    UmiWebSseResponseDestroy(response);
    if(fflush(stdout)!=0 || ferror(stdout))return 3;
    return 0;
failed:
    fprintf(stderr,"SSE reader failed: %s\n",umi_status_text(status));
    UmiWebSseParserDestroy(parser);
    UmiWebSseResponseDestroy(response);
    return 2;
}
