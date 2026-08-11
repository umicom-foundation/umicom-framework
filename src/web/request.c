/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/request.c
 *
 * PURPOSE:
 *   Implement request initialisation, target splitting and header lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/request.h"
#include <string.h>
void umi_web_request_init(UmiWebRequest *request){if(request==NULL)return;(void)memset(request,0,sizeof(*request));request->method=UMI_HTTP_METHOD_UNKNOWN;(void)umi_web_copy_text(request->version,sizeof(request->version),"HTTP/1.1");}
UmiStatus umi_web_request_set_target(UmiWebRequest *request,const char *target){const char *q;size_t path_len;if(request==NULL||target==NULL)return UMI_STATUS_INVALID_ARGUMENT;q=strchr(target,'?');if(q==NULL){return umi_web_copy_text(request->path,sizeof(request->path),target);}path_len=(size_t)(q-target);if(path_len+1U>sizeof(request->path))return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(request->path,target,path_len);request->path[path_len]='\0';return umi_web_copy_text(request->query,sizeof(request->query),q+1);}
const char *umi_web_request_header(const UmiWebRequest *request,const char *name){size_t i;if(request==NULL||name==NULL)return NULL;for(i=0U;i<request->header_count;++i)if(umi_web_header_name_equal(&request->headers[i],name))return request->headers[i].value;return NULL;}
