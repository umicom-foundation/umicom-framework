/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/response.c
 *
 * PURPOSE:
 *   Implement HTTP response helpers and wire-format serialisation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/response.h"
#include <stdio.h>
#include <string.h>
void umi_web_response_init(UmiWebResponse *response){if(response==NULL)return;(void)memset(response,0,sizeof(*response));response->status=UMI_HTTP_STATUS_OK;}
UmiStatus umi_web_response_set_header(UmiWebResponse *response,const char *name,const char *value){if(response==NULL||response->header_count>=UMI_WEB_MAX_HEADERS)return response==NULL?UMI_STATUS_INVALID_ARGUMENT:UMI_STATUS_CAPACITY_EXCEEDED;return umi_web_header_set(&response->headers[response->header_count++],name,value);}
UmiStatus umi_web_response_set_text(UmiWebResponse *response,int status,const char *content_type,const char *body){size_t n;UmiStatus s;if(response==NULL||body==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_web_response_init(response);response->status=status;s=umi_web_response_set_header(response,"Content-Type",content_type!=NULL?content_type:"text/plain; charset=utf-8");if(s!=UMI_STATUS_OK)return s;n=strlen(body);if(n+1U>sizeof(response->body))return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(response->body,body,n+1U);response->body_length=n;return UMI_STATUS_OK;}
UmiStatus umi_web_response_format(const UmiWebResponse *response,char *out_text,size_t capacity,size_t *out_length){size_t used=0U,i;int n;if(response==NULL||out_text==NULL||capacity==0U)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(out_text,capacity,"HTTP/1.1 %d %s\r\n",response->status,umi_http_status_reason(response->status));if(n<0||(size_t)n>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;used=(size_t)n;for(i=0U;i<response->header_count;++i){n=snprintf(out_text+used,capacity-used,"%s: %s\r\n",response->headers[i].name,response->headers[i].value);if(n<0||(size_t)n>=capacity-used)return UMI_STATUS_CAPACITY_EXCEEDED;used+=(size_t)n;}n=snprintf(out_text+used,capacity-used,"Content-Length: %zu\r\n\r\n",response->body_length);if(n<0||(size_t)n>=capacity-used)return UMI_STATUS_CAPACITY_EXCEEDED;used+=(size_t)n;if(response->body_length+1U>capacity-used)return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(out_text+used,response->body,response->body_length);used+=response->body_length;out_text[used]='\0';if(out_length!=NULL)*out_length=used;return UMI_STATUS_OK;}
