/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/framing.c
 *
 * PURPOSE:
 *   Implement non-mutating, bounded Content-Length framing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/framing.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void umi_language_runtime_framer_init(UmiLanguageRuntimeFramer*f){if(f)memset(f,0,sizeof(*f));}
UmiStatus umi_language_runtime_frame_encode(const char*j,char*out,size_t cap,size_t*len){size_t n;int h;if(j==NULL||out==NULL||cap==0||len==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=strlen(j);h=snprintf(out,cap,"Content-Length: %zu\r\n\r\n",n);if(h<0||(size_t)h+n+1>cap)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(out+(size_t)h,j,n);out[(size_t)h+n]=0;*len=(size_t)h+n;return UMI_STATUS_OK;}
static UmiStatus header(UmiLanguageRuntimeFramer*f){const char*sep,*cur,*end;size_t hlen=0,body=0;int found=0;sep=strstr(f->buffer,"\r\n\r\n");if(!sep)return UMI_STATUS_NOT_FOUND;hlen=(size_t)(sep-f->buffer)+4U;cur=f->buffer;while(cur<sep){end=strstr(cur,"\r\n");if(!end||end>sep)end=sep;size_t n=(size_t)(end-cur);if(n>=15U&&strncmp(cur,"Content-Length:",15U)==0){char b[32],*e;size_t s=15U,m;while(s<n&&isspace((unsigned char)cur[s]))s++;m=n-s;if(m==0||m>=sizeof(b))return UMI_STATUS_PARSE_ERROR;memcpy(b,cur+s,m);b[m]=0;unsigned long long v=strtoull(b,&e,10);if(e==b||*e||v>UMI_LANGUAGE_RUNTIME_JSON_CAPACITY-1U)return UMI_STATUS_PARSE_ERROR;body=(size_t)v;found=1;}cur=end+2;}if(!found)return UMI_STATUS_PARSE_ERROR;f->header_length=hlen;f->expected_body=body;f->header_ready=1;return UMI_STATUS_OK;}
UmiStatus umi_language_runtime_framer_feed(UmiLanguageRuntimeFramer*f,const void*b,size_t n){UmiStatus s;if(f==NULL||(b==NULL&&n))return UMI_STATUS_INVALID_ARGUMENT;if(f->length+n>=sizeof(f->buffer))return UMI_STATUS_CAPACITY_EXCEEDED;if(n){memcpy(f->buffer+f->length,b,n);f->length+=n;}f->buffer[f->length]=0;if(!f->header_ready){s=header(f);if(s!=UMI_STATUS_OK&&s!=UMI_STATUS_NOT_FOUND)return s;}return UMI_STATUS_OK;}
int umi_language_runtime_framer_has_message(const UmiLanguageRuntimeFramer*f){return f&&f->header_ready&&f->length>=f->header_length+f->expected_body;}
UmiStatus umi_language_runtime_framer_pop(UmiLanguageRuntimeFramer*f,char*out,size_t cap,size_t*len){size_t used,remain;UmiStatus s;if(f==NULL||out==NULL||cap==0||len==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(!umi_language_runtime_framer_has_message(f))return UMI_STATUS_NOT_FOUND;if(f->expected_body+1>cap)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(out,f->buffer+f->header_length,f->expected_body);out[f->expected_body]=0;*len=f->expected_body;used=f->header_length+f->expected_body;remain=f->length-used;if(remain)memmove(f->buffer,f->buffer+used,remain);f->length=remain;f->buffer[remain]=0;f->expected_body=f->header_length=0;f->header_ready=0;if(remain){s=header(f);if(s!=UMI_STATUS_OK&&s!=UMI_STATUS_NOT_FOUND)return s;}return UMI_STATUS_OK;}
