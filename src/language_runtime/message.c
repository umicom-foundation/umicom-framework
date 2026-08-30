/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/message.c
 *
 * PURPOSE:
 *   Implement JSON-RPC envelope construction and response correlation metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/message.h"
#include <stdio.h>
#include <string.h>
static UmiStatus build(int has,uint64_t id,const char*m,const char*p,char*out,size_t cap){UmiLanguageRuntimeJsonWriter w;if(m==NULL||!*m||out==NULL||cap==0)return UMI_STATUS_INVALID_ARGUMENT;umi_language_runtime_json_writer_init(&w,out,cap);umi_language_runtime_json_writer_raw(&w,"{\"jsonrpc\":\"2.0\"");if(has){umi_language_runtime_json_writer_raw(&w,",\"id\":");umi_language_runtime_json_writer_uint64(&w,id);}umi_language_runtime_json_writer_raw(&w,",\"method\":");umi_language_runtime_json_writer_string(&w,m);if(p){umi_language_runtime_json_writer_raw(&w,",\"params\":");umi_language_runtime_json_writer_raw(&w,p);}umi_language_runtime_json_writer_raw(&w,"}");return w.status;}
UmiStatus umi_language_runtime_build_request(uint64_t id,const char*m,const char*p,char*out,size_t cap){return id?build(1,id,m,p,out,cap):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_language_runtime_build_notification(const char*m,const char*p,char*out,size_t cap){return build(0,0,m,p,out,cap);}
UmiStatus umi_language_runtime_message_parse(const char*j,UmiLanguageRuntimeEnvelope*out){UmiLanguageRuntimeJsonDocument d;int mt,it,rt,et;UmiStatus s;if(j==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);if(s!=UMI_STATUS_OK)return s;if(!d.token_count||d.tokens[0].type!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT)return UMI_STATUS_PARSE_ERROR;mt=umi_language_runtime_json_object_get(&d,0,"method");it=umi_language_runtime_json_object_get(&d,0,"id");rt=umi_language_runtime_json_object_get(&d,0,"result");et=umi_language_runtime_json_object_get(&d,0,"error");if(mt>=0){if(umi_language_runtime_json_string(&d,mt,out->method,sizeof(out->method))!=UMI_STATUS_OK)return UMI_STATUS_PARSE_ERROR;out->kind=it>=0?UMI_LANGUAGE_RUNTIME_MESSAGE_REQUEST:UMI_LANGUAGE_RUNTIME_MESSAGE_NOTIFICATION;}else if(et>=0)out->kind=UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR;else if(rt>=0||it>=0)out->kind=UMI_LANGUAGE_RUNTIME_MESSAGE_RESPONSE;else out->kind=UMI_LANGUAGE_RUNTIME_MESSAGE_UNKNOWN;if(it>=0&&!umi_language_runtime_json_is_null(&d,it)){int64_t v=0;if(umi_language_runtime_json_int64(&d,it,&v)==UMI_STATUS_OK&&v>0)out->request_id=(uint64_t)v;}if(et>=0&&d.tokens[et].type==UMI_LANGUAGE_RUNTIME_JSON_OBJECT){int c=umi_language_runtime_json_object_get(&d,et,"code"),m=umi_language_runtime_json_object_get(&d,et,"message");int64_t v=0;if(c>=0&&umi_language_runtime_json_int64(&d,c,&v)==UMI_STATUS_OK)out->error_code=(int)v;if(m>=0)umi_language_runtime_json_string(&d,m,out->error_message,sizeof(out->error_message));}if(strlen(j)>=sizeof(out->json))return UMI_STATUS_CAPACITY_EXCEEDED;snprintf(out->json,sizeof(out->json),"%s",j);return UMI_STATUS_OK;}
