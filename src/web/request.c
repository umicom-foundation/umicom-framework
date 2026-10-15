/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/request.c
 *
 * PURPOSE:
 *   Implement request initialisation, target splitting and header lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/request.h"
#include <string.h>
#include <stdlib.h>
/*
 * Initialise web request from caller-provided values so later operations receive a known
 * state.
 */
void umi_web_request_init(UmiWebRequest *request){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(request==NULL)return;(void)memset(request,0,sizeof(*request));request->method=UMI_HTTP_METHOD_UNKNOWN;(void)umi_web_copy_text(request->version,sizeof(request->version),"HTTP/1.1");}
/*
 * Provide the web request set target operation used by this module and its client
 * applications.
 */
/* The earlier split published path before validating query and retained stale
 * query text for a later query-free target. Retain it for review; the bounded
 * replacement below publishes both fields together after accepting the target. */
#if 0
UmiStatus umi_web_request_set_target(UmiWebRequest *request,const char *target){const char *q;size_t path_len;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(request==NULL||target==NULL)return UMI_STATUS_INVALID_ARGUMENT;q=strchr(target,'?');/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL){return umi_web_copy_text(request->path,sizeof(request->path),target);}path_len=(size_t)(q-target);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(path_len+1U>sizeof(request->path))return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(request->path,target,path_len);request->path[path_len]='\0';return umi_web_copy_text(request->query,sizeof(request->query),q+1);}
#endif

/* Keep both target fields together until every length is known. A target may
 * borrow request storage, so writing path before reading query would be unsafe.
 * Only target text is staged; headers and the potentially large body stay put. */
UmiStatus umi_web_request_set_target(UmiWebRequest *request, const char *target)
{
    if (request == NULL || target == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t path_length = 0U;
    while (path_length < sizeof(request->path) &&
           target[path_length] != '\0' && target[path_length] != '?')
        ++path_length;
    if (path_length == sizeof(request->path)) return UMI_STATUS_CAPACITY_EXCEEDED;

    const char *query = target[path_length] == '?' ? target + path_length + 1U : "";
    size_t query_length = 0U;
    while (query_length < sizeof(request->query) && query[query_length] != '\0')
        ++query_length;
    if (query_length == sizeof(request->query)) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* A bounded pair avoids copying unrelated request state. Allocation failure
     * and validation failure both leave the complete previous request intact. */
    struct TargetText {
        char path[UMI_WEB_PATH_CAPACITY];
        char query[UMI_WEB_QUERY_CAPACITY];
    } *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(candidate->path, target, path_length);
    candidate->path[path_length] = '\0';
    memcpy(candidate->query, query, query_length + 1U);
    memcpy(request->path, candidate->path, path_length + 1U);
    memcpy(request->query, candidate->query, query_length + 1U);
    free(candidate);
    return UMI_STATUS_OK;
}
/*
 * Provide the web request header operation used by this module and its client
 * applications.
 */
const char *umi_web_request_header(const UmiWebRequest *request,const char *name){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(request==NULL||name==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<request->header_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_web_header_name_equal(&request->headers[i],name))return request->headers[i].value;return NULL;}
