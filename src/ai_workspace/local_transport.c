/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/local_transport.c
 *
 * PURPOSE:
 *   Retain native HTTP global lifetime until all local request owners are released.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_transport_internal.h"
#include "umicom/platform/threading.h"
#include <stdint.h>
#include <stdlib.h>
#ifdef UMICOM_AI_WORKSPACE_HAS_HTTP
#include <curl/curl.h>
#endif
struct UmiAiLocalTransport {
    uint64_t owner_thread;
    size_t requests;
};
static UmiStatus Owner(UmiAiLocalTransport *transport)
{
    if(transport==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    return transport->owner_thread==umi_thread_current_id()?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
}
UmiStatus UmiAiLocalTransportCreate(UmiAiLocalTransport **out)
{
    if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
#ifdef UMICOM_AI_WORKSPACE_HAS_HTTP
    UmiAiLocalTransport *transport=calloc(1U,sizeof *transport);
    if(transport==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    /* Startup ordering is a caller precondition: version_info itself is not
     * thread-safe before global initialization, so probing first is unsafe. */
    if(curl_global_init(CURL_GLOBAL_DEFAULT)!=CURLE_OK){
        free(transport);return UMI_STATUS_UNAVAILABLE;
    }
    const curl_version_info_data *version=curl_version_info(CURLVERSION_NOW);
    if(version==NULL || (version->features&CURL_VERSION_THREADSAFE)==0){
        curl_global_cleanup();free(transport);return UMI_STATUS_UNAVAILABLE;
    }
    transport->owner_thread=umi_thread_current_id();
    *out=transport;return UMI_STATUS_OK;
#else
    return UMI_STATUS_UNAVAILABLE;
#endif
}
UmiStatus AwLocalTransportRetain(UmiAiLocalTransport *transport)
{
    UmiStatus status=Owner(transport);if(status!=UMI_STATUS_OK)return status;
    if(transport->requests==SIZE_MAX)return UMI_STATUS_CAPACITY_EXCEEDED;
    ++transport->requests;return UMI_STATUS_OK;
}
UmiStatus AwLocalTransportRelease(UmiAiLocalTransport *transport)
{
    UmiStatus status=Owner(transport);if(status!=UMI_STATUS_OK)return status;
    if(transport->requests==0U)return UMI_STATUS_INVALID_STATE;
    --transport->requests;return UMI_STATUS_OK;
}
UmiStatus UmiAiLocalTransportRequestCount(UmiAiLocalTransport *transport,size_t *out)
{
    if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=Owner(transport);if(status!=UMI_STATUS_OK)return status;
    *out=transport->requests;return UMI_STATUS_OK;
}
UmiStatus UmiAiLocalTransportDestroy(UmiAiLocalTransport **in_out)
{
    if(in_out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiLocalTransport *transport=*in_out;if(transport==NULL)return UMI_STATUS_OK;
    UmiStatus status=Owner(transport);if(status!=UMI_STATUS_OK)return status;
    if(transport->requests!=0U)return UMI_STATUS_BUSY;
#ifdef UMICOM_AI_WORKSPACE_HAS_HTTP
    curl_global_cleanup();
#endif
    free(transport);*in_out=NULL;return UMI_STATUS_OK;
}
