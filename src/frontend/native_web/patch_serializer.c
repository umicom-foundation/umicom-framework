/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/patch_serializer.c
 *
 * PURPOSE:
 *   Serialize patch batches into the compact native-web JSON patch protocol.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/patch_serializer.h"

#include <stdio.h>
static UmiStatus json_string(UmiNativeWebMarkupBuffer *b,const char *s){const char *p;UmiStatus st;if(s==NULL)s="";st=umi_native_web_markup_buffer_append(b,"\"");if(st!=UMI_STATUS_OK)return st;for(p=s;*p!='\0';++p){if(*p=='\"'||*p=='\\'){st=umi_native_web_markup_buffer_append_n(b,"\\",1U);if(st!=UMI_STATUS_OK)return st;}if((unsigned char)*p<0x20U)return UMI_STATUS_PARSE_ERROR;st=umi_native_web_markup_buffer_append_n(b,p,1U);if(st!=UMI_STATUS_OK)return st;}return umi_native_web_markup_buffer_append(b,"\"");}
UmiStatus umi_native_web_patch_serialize(const UmiNativeWebPatchBatch *batch,UmiNativeWebMarkupBuffer *out_json){size_t i;char n[64];UmiStatus s;if(batch==NULL||out_json==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_native_web_markup_buffer_init(out_json);s=umi_native_web_markup_buffer_append(out_json,"[");if(s!=UMI_STATUS_OK)return s;for(i=0;i<batch->count;++i){if(i>0U){s=umi_native_web_markup_buffer_append(out_json,",");if(s!=UMI_STATUS_OK)return s;}s=umi_native_web_markup_buffer_append(out_json,"{\"k\":");if(s!=UMI_STATUS_OK)return s;(void)snprintf(n,sizeof(n),"%d",(int)batch->items[i].kind);s=umi_native_web_markup_buffer_append(out_json,n);if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_json,",\"t\":");if(s!=UMI_STATUS_OK)return s;s=json_string(out_json,batch->items[i].target_id);if(s!=UMI_STATUS_OK)return s;if(batch->items[i].name[0]!='\0'){s=umi_native_web_markup_buffer_append(out_json,",\"n\":");if(s!=UMI_STATUS_OK)return s;s=json_string(out_json,batch->items[i].name);if(s!=UMI_STATUS_OK)return s;}if(batch->items[i].value[0]!='\0'){s=umi_native_web_markup_buffer_append(out_json,",\"v\":");if(s!=UMI_STATUS_OK)return s;s=json_string(out_json,batch->items[i].value);if(s!=UMI_STATUS_OK)return s;}s=umi_native_web_markup_buffer_append(out_json,"}");if(s!=UMI_STATUS_OK)return s;}return umi_native_web_markup_buffer_append(out_json,"]");}

