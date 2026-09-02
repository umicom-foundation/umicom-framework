/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/patch_serializer.c
 *
 * PURPOSE:
 *   Serialize patch batches into the compact native-web JSON patch protocol.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/patch_serializer.h"

#include <stdio.h>
/* Provide the json string operation used by this module and its client applications. */
static UmiStatus json_string(UmiNativeWebMarkupBuffer *b,const char *s){const char *p;UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)s="";st=umi_native_web_markup_buffer_append(b,"\"");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;/* Visit each bounded item once so every record receives the same rule. */ for(p=s;*p!='\0';++p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(*p=='\"'||*p=='\\'){st=umi_native_web_markup_buffer_append_n(b,"\\",1U);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if((unsigned char)*p<0x20U)return UMI_STATUS_PARSE_ERROR;st=umi_native_web_markup_buffer_append_n(b,p,1U);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;}return umi_native_web_markup_buffer_append(b,"\"");}
/*
 * Write native web patch in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_native_web_patch_serialize(const UmiNativeWebPatchBatch *batch,UmiNativeWebMarkupBuffer *out_json){size_t i;char n[64];UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(batch==NULL||out_json==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_native_web_markup_buffer_init(out_json);s=umi_native_web_markup_buffer_append(out_json,"[");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<batch->count;++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i>0U){s=umi_native_web_markup_buffer_append(out_json,",");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}s=umi_native_web_markup_buffer_append(out_json,"{\"k\":");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;(void)snprintf(n,sizeof(n),"%d",(int)batch->items[i].kind);s=umi_native_web_markup_buffer_append(out_json,n);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_json,",\"t\":");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=json_string(out_json,batch->items[i].target_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(batch->items[i].name[0]!='\0'){s=umi_native_web_markup_buffer_append(out_json,",\"n\":");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=json_string(out_json,batch->items[i].name);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(batch->items[i].value[0]!='\0'){s=umi_native_web_markup_buffer_append(out_json,",\"v\":");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=json_string(out_json,batch->items[i].value);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}s=umi_native_web_markup_buffer_append(out_json,"}");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}return umi_native_web_markup_buffer_append(out_json,"]");}

