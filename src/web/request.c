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
/*
 * Initialise web request from caller-provided values so later operations receive a known
 * state.
 */
void umi_web_request_init(UmiWebRequest *request){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(request==NULL)return;(void)memset(request,0,sizeof(*request));request->method=UMI_HTTP_METHOD_UNKNOWN;(void)umi_web_copy_text(request->version,sizeof(request->version),"HTTP/1.1");}
/*
 * Provide the web request set target operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_request_set_target(UmiWebRequest *request,const char *target){const char *q;size_t path_len;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(request==NULL||target==NULL)return UMI_STATUS_INVALID_ARGUMENT;q=strchr(target,'?');/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL){return umi_web_copy_text(request->path,sizeof(request->path),target);}path_len=(size_t)(q-target);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(path_len+1U>sizeof(request->path))return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(request->path,target,path_len);request->path[path_len]='\0';return umi_web_copy_text(request->query,sizeof(request->query),q+1);}
/*
 * Provide the web request header operation used by this module and its client
 * applications.
 */
const char *umi_web_request_header(const UmiWebRequest *request,const char *name){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(request==NULL||name==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<request->header_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_web_header_name_equal(&request->headers[i],name))return request->headers[i].value;return NULL;}
