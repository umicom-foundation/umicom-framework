/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/local_http_policy.c
 *
 * PURPOSE:
 *   Centralize checked loopback route, deadline, proxy and protocol policy for native AI requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_http_internal.h"
#ifdef UMICOM_AI_WORKSPACE_HAS_HTTP
#include <stdio.h>
#include <string.h>
UmiStatus AwLocalHttpConfigure(CURL *curl,uint16_t port,uint32_t timeout_millis,
    const char *body,struct curl_slist *headers,const AwLocalHttpCallbacks *callbacks,bool streaming)
{
    if(curl==NULL || port<1024U || timeout_millis<100U || timeout_millis>60000U ||
        callbacks==NULL || callbacks->write==NULL || callbacks->progress==NULL ||
        (body!=NULL && headers==NULL) ||
        (streaming && (body==NULL || callbacks->header==NULL)))return UMI_STATUS_INVALID_ARGUMENT;
    char url[96];
    int written=snprintf(url,sizeof url,"http://127.0.0.1:%u%s",(unsigned)port,
        body==NULL?"/health":"/v1/chat/completions");
    if(written<0 || (size_t)written>=sizeof url)return UMI_STATUS_INTERNAL_ERROR;
#define AW_LOCAL_CURL(option,value) do{if(curl_easy_setopt(curl,option,value)!=CURLE_OK)return UMI_STATUS_UNAVAILABLE;}while(0)
    AW_LOCAL_CURL(CURLOPT_URL,url);
    if(body!=NULL){
        AW_LOCAL_CURL(CURLOPT_HTTPHEADER,headers);AW_LOCAL_CURL(CURLOPT_POST,1L);
        AW_LOCAL_CURL(CURLOPT_POSTFIELDS,body);
        AW_LOCAL_CURL(CURLOPT_POSTFIELDSIZE_LARGE,(curl_off_t)strlen(body));
    }else AW_LOCAL_CURL(CURLOPT_HTTPGET,1L);
    AW_LOCAL_CURL(CURLOPT_PROXY,"");AW_LOCAL_CURL(CURLOPT_NOPROXY,"*");
    AW_LOCAL_CURL(CURLOPT_FOLLOWLOCATION,0L);AW_LOCAL_CURL(CURLOPT_MAXREDIRS,0L);
    AW_LOCAL_CURL(CURLOPT_PROTOCOLS_STR,"http");AW_LOCAL_CURL(CURLOPT_REDIR_PROTOCOLS_STR,"http");
    AW_LOCAL_CURL(CURLOPT_NETRC,(long)CURL_NETRC_IGNORED);AW_LOCAL_CURL(CURLOPT_NOSIGNAL,1L);
    AW_LOCAL_CURL(CURLOPT_CONNECTTIMEOUT_MS,(long)(timeout_millis<3000U?timeout_millis:3000U));
    AW_LOCAL_CURL(CURLOPT_TIMEOUT_MS,(long)timeout_millis);
    AW_LOCAL_CURL(CURLOPT_WRITEFUNCTION,callbacks->write);AW_LOCAL_CURL(CURLOPT_WRITEDATA,callbacks->write_context);
    AW_LOCAL_CURL(CURLOPT_NOPROGRESS,0L);AW_LOCAL_CURL(CURLOPT_XFERINFOFUNCTION,callbacks->progress);
    AW_LOCAL_CURL(CURLOPT_XFERINFODATA,callbacks->progress_context);
    if(callbacks->header!=NULL){
        AW_LOCAL_CURL(CURLOPT_HEADERFUNCTION,callbacks->header);
        AW_LOCAL_CURL(CURLOPT_HEADERDATA,callbacks->header_context);
    }
    if(streaming){
        /* Match the shared HTTP/1 response admission profile explicitly.
         * Do not silently decompress an unsupported response representation. */
        AW_LOCAL_CURL(CURLOPT_HTTP_VERSION,(long)CURL_HTTP_VERSION_1_1);
        AW_LOCAL_CURL(CURLOPT_ACCEPT_ENCODING,"identity");
        AW_LOCAL_CURL(CURLOPT_HTTP_CONTENT_DECODING,0L);
    }
#undef AW_LOCAL_CURL
    return UMI_STATUS_OK;
}
#endif
