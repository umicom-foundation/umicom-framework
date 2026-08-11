/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/parser.c
 *
 * PURPOSE:
 *   Implement a bounded HTTP/1.x request parser.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/parser.h"
#include <string.h>
static const char *find_crlf(const char *p,const char *end){for(;p+1<end;++p)if(p[0]=='\r'&&p[1]=='\n')return p;return NULL;}
UmiStatus umi_web_parse_request(const char *text,size_t length,UmiWebRequest *out_request){const char *p=text,*end=text+length,*line_end,*space1,*space2,*headers_end;char method[16],target[UMI_WEB_PATH_CAPACITY+UMI_WEB_QUERY_CAPACITY];size_t n;UmiStatus s;if(text==NULL||out_request==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_web_request_init(out_request);line_end=find_crlf(p,end);if(line_end==NULL)return UMI_STATUS_PARSE_ERROR;space1=memchr(p,' ',(size_t)(line_end-p));if(space1==NULL)return UMI_STATUS_PARSE_ERROR;space2=memchr(space1+1,' ',(size_t)(line_end-space1-1));if(space2==NULL)return UMI_STATUS_PARSE_ERROR;n=(size_t)(space1-p);if(n+1U>sizeof(method))return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(method,p,n);method[n]='\0';out_request->method=umi_http_method_from_text(method);if(out_request->method==UMI_HTTP_METHOD_UNKNOWN)return UMI_STATUS_PARSE_ERROR;n=(size_t)(space2-space1-1);if(n+1U>sizeof(target))return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(target,space1+1,n);target[n]='\0';s=umi_web_request_set_target(out_request,target);if(s!=UMI_STATUS_OK)return s;n=(size_t)(line_end-space2-1);if(n+1U>sizeof(out_request->version))return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(out_request->version,space2+1,n);out_request->version[n]='\0';p=line_end+2;headers_end=NULL;while(p<end){line_end=find_crlf(p,end);if(line_end==NULL)return UMI_STATUS_PARSE_ERROR;if(line_end==p){headers_end=line_end+2;break;}if(out_request->header_count>=UMI_WEB_MAX_HEADERS)return UMI_STATUS_CAPACITY_EXCEEDED;{const char *colon=memchr(p,':',(size_t)(line_end-p));size_t nn,vn;if(colon==NULL)return UMI_STATUS_PARSE_ERROR;nn=(size_t)(colon-p);while(colon+1<line_end&&colon[1]==' ')++colon;vn=(size_t)(line_end-colon-1);if(nn+1U>UMI_WEB_HEADER_NAME_CAPACITY||vn+1U>UMI_WEB_HEADER_VALUE_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(out_request->headers[out_request->header_count].name,p,nn);out_request->headers[out_request->header_count].name[nn]='\0';(void)memcpy(out_request->headers[out_request->header_count].value,colon+1,vn);out_request->headers[out_request->header_count].value[vn]='\0';++out_request->header_count;}p=line_end+2;}if(headers_end==NULL)return UMI_STATUS_PARSE_ERROR;n=(size_t)(end-headers_end);if(n>=sizeof(out_request->body))return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(out_request->body,headers_end,n);out_request->body[n]='\0';out_request->body_length=n;return UMI_STATUS_OK;}
