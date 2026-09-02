/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/json_writer.c
 *
 * PURPOSE:
 *   Implement bounded JSON emission for LSP request builders.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json_writer.h"
#include <stdio.h>
#include <string.h>
/* Provide the ap operation used by this module and its client applications. */
static UmiStatus ap(UmiLanguageRuntimeJsonWriter*w,const char*t,size_t n){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(w==NULL||t==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(w->status!=UMI_STATUS_OK)return w->status;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(w->length+n+1>w->capacity)return w->status=UMI_STATUS_CAPACITY_EXCEEDED;memcpy(w->buffer+w->length,t,n);w->length+=n;w->buffer[w->length]=0;return UMI_STATUS_OK;}
/*
 * Initialise language runtime json writer from caller-provided values so later operations
 * receive a known state.
 */
void umi_language_runtime_json_writer_init(UmiLanguageRuntimeJsonWriter*w,char*b,size_t c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(w==NULL)return;w->buffer=b;w->capacity=c;w->length=0;w->status=(b!=NULL&&c>0)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b!=NULL&&c>0)b[0]=0;}
/*
 * Provide the language runtime json writer raw operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_json_writer_raw(UmiLanguageRuntimeJsonWriter*w,const char*t){return t?ap(w,t,strlen(t)):UMI_STATUS_INVALID_ARGUMENT;}
/*
 * Provide the language runtime json writer string operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_json_writer_string(UmiLanguageRuntimeJsonWriter*w,const char*t){const unsigned char*p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(w==NULL||t==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ap(w,"\"",1)!=UMI_STATUS_OK)return w->status;/* Visit each bounded item once so every record receives the same rule. */ for(p=(const unsigned char*)t;*p;p++){char b[8];const char*s=b;size_t n=1;/* Select the behaviour associated with the requested command or state value. */ switch(*p){case'"':s="\\\"";n=2;break;case'\\':s="\\\\";n=2;break;case'\n':s="\\n";n=2;break;case'\r':s="\\r";n=2;break;case'\t':s="\\t";n=2;break;default:/* Protect caller-owned memory by checking that required state is available before it is used. */ if(*p<0x20U){int x=snprintf(b,sizeof(b),"\\u%04x",(unsigned)*p);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(x!=6)return w->status=UMI_STATUS_INTERNAL_ERROR;n=6;}/* Use this fallback path when the earlier condition does not apply. */ else b[0]=(char)*p;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ap(w,s,n)!=UMI_STATUS_OK)return w->status;}return ap(w,"\"",1);}
/*
 * Provide the language runtime json writer uint64 operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_json_writer_uint64(UmiLanguageRuntimeJsonWriter*w,uint64_t v){char b[32];int n=snprintf(b,sizeof(b),"%llu",(unsigned long long)v);return n<0||(size_t)n>=sizeof(b)?UMI_STATUS_INTERNAL_ERROR:ap(w,b,(size_t)n);}
/*
 * Provide the language runtime json writer bool operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_json_writer_bool(UmiLanguageRuntimeJsonWriter*w,int v){return ap(w,v?"true":"false",v?4U:5U);}
/*
 * Provide the language runtime json writer int64 operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_json_writer_int64(UmiLanguageRuntimeJsonWriter*w,int64_t v){char b[32];int n=snprintf(b,sizeof(b),"%lld",(long long)v);return n<0||(size_t)n>=sizeof(b)?UMI_STATUS_INTERNAL_ERROR:ap(w,b,(size_t)n);}
