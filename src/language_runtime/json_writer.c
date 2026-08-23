/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/json_writer.c
 *
 * PURPOSE:
 *   Implement bounded JSON emission for LSP request builders.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json_writer.h"
#include <stdio.h>
#include <string.h>
static UmiStatus ap(UmiLanguageRuntimeJsonWriter*w,const char*t,size_t n){if(w==NULL||t==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(w->status!=UMI_STATUS_OK)return w->status;if(w->length+n+1>w->capacity)return w->status=UMI_STATUS_CAPACITY_EXCEEDED;memcpy(w->buffer+w->length,t,n);w->length+=n;w->buffer[w->length]=0;return UMI_STATUS_OK;}
void umi_language_runtime_json_writer_init(UmiLanguageRuntimeJsonWriter*w,char*b,size_t c){if(w==NULL)return;w->buffer=b;w->capacity=c;w->length=0;w->status=(b!=NULL&&c>0)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;if(b!=NULL&&c>0)b[0]=0;}
UmiStatus umi_language_runtime_json_writer_raw(UmiLanguageRuntimeJsonWriter*w,const char*t){return t?ap(w,t,strlen(t)):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_language_runtime_json_writer_string(UmiLanguageRuntimeJsonWriter*w,const char*t){const unsigned char*p;if(w==NULL||t==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(ap(w,"\"",1)!=UMI_STATUS_OK)return w->status;for(p=(const unsigned char*)t;*p;p++){char b[8];const char*s=b;size_t n=1;switch(*p){case'"':s="\\\"";n=2;break;case'\\':s="\\\\";n=2;break;case'\n':s="\\n";n=2;break;case'\r':s="\\r";n=2;break;case'\t':s="\\t";n=2;break;default:if(*p<0x20U){int x=snprintf(b,sizeof(b),"\\u%04x",(unsigned)*p);if(x!=6)return w->status=UMI_STATUS_INTERNAL_ERROR;n=6;}else b[0]=(char)*p;}if(ap(w,s,n)!=UMI_STATUS_OK)return w->status;}return ap(w,"\"",1);}
UmiStatus umi_language_runtime_json_writer_uint64(UmiLanguageRuntimeJsonWriter*w,uint64_t v){char b[32];int n=snprintf(b,sizeof(b),"%llu",(unsigned long long)v);return n<0||(size_t)n>=sizeof(b)?UMI_STATUS_INTERNAL_ERROR:ap(w,b,(size_t)n);}
UmiStatus umi_language_runtime_json_writer_bool(UmiLanguageRuntimeJsonWriter*w,int v){return ap(w,v?"true":"false",v?4U:5U);}
