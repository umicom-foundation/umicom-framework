/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/framing.c
 *
 * PURPOSE:
 *   Implement non-mutating, bounded Content-Length framing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/framing.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * Initialise language runtime framer from caller-provided values so later operations
 * receive a known state.
 */
void umi_language_runtime_framer_init(UmiLanguageRuntimeFramer*f){/* Apply this branch only when its contract condition is satisfied. */ if(f)memset(f,0,sizeof(*f));}
/*
 * Write language runtime frame in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_language_runtime_frame_encode(const char*j,char*out,size_t cap,size_t*len){size_t n;int h;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(j==NULL||out==NULL||cap==0||len==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=strlen(j);h=snprintf(out,cap,"Content-Length: %zu\r\n\r\n",n);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h<0||(size_t)h+n+1>cap)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(out+(size_t)h,j,n);out[(size_t)h+n]=0;*len=(size_t)h+n;return UMI_STATUS_OK;}
/* Provide the header operation used by this module and its client applications. */
static UmiStatus header(UmiLanguageRuntimeFramer*f){const char*sep,*cur,*end;size_t hlen=0,body=0;int found=0;sep=strstr(f->buffer,"\r\n\r\n");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!sep)return UMI_STATUS_NOT_FOUND;hlen=(size_t)(sep-f->buffer)+4U;cur=f->buffer;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(cur<sep){end=strstr(cur,"\r\n");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!end||end>sep)end=sep;size_t n=(size_t)(end-cur);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n>=15U&&strncmp(cur,"Content-Length:",15U)==0){char b[32],*e;size_t s=15U,m;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(s<n&&isspace((unsigned char)cur[s]))s++;m=n-s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(m==0||m>=sizeof(b))return UMI_STATUS_PARSE_ERROR;memcpy(b,cur+s,m);b[m]=0;unsigned long long v=strtoull(b,&e,10);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(e==b||*e||v>UMI_LANGUAGE_RUNTIME_JSON_CAPACITY-1U)return UMI_STATUS_PARSE_ERROR;body=(size_t)v;found=1;}cur=end+2;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!found)return UMI_STATUS_PARSE_ERROR;f->header_length=hlen;f->expected_body=body;f->header_ready=1;return UMI_STATUS_OK;}
/*
 * Provide the language runtime framer feed operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_framer_feed(UmiLanguageRuntimeFramer*f,const void*b,size_t n){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||(b==NULL&&n))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f->length+n>=sizeof(f->buffer))return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n){memcpy(f->buffer+f->length,b,n);f->length+=n;}f->buffer[f->length]=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!f->header_ready){s=header(f);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK&&s!=UMI_STATUS_NOT_FOUND)return s;}return UMI_STATUS_OK;}
/*
 * Provide the language runtime framer has message operation used by this module and its
 * client applications.
 */
int umi_language_runtime_framer_has_message(const UmiLanguageRuntimeFramer*f){return f&&f->header_ready&&f->length>=f->header_length+f->expected_body;}
/*
 * Provide the language runtime framer pop operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_framer_pop(UmiLanguageRuntimeFramer*f,char*out,size_t cap,size_t*len){size_t used,remain;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||out==NULL||cap==0||len==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_language_runtime_framer_has_message(f))return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f->expected_body+1>cap)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(out,f->buffer+f->header_length,f->expected_body);out[f->expected_body]=0;*len=f->expected_body;used=f->header_length+f->expected_body;remain=f->length-used;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(remain)memmove(f->buffer,f->buffer+used,remain);f->length=remain;f->buffer[remain]=0;f->expected_body=f->header_length=0;f->header_ready=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(remain){s=header(f);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK&&s!=UMI_STATUS_NOT_FOUND)return s;}return UMI_STATUS_OK;}
