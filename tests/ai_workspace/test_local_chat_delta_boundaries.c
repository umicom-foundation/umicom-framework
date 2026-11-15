/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_chat_delta_boundaries.c
 *
 * PURPOSE:
 *   Check terminal marker precision, codec limits and shared request admission for both stream modes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_chat_internal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!UmiAiWorkspaceLocalProviderAvailable()){
        fputs("SKIP: local HTTP/json-c support is unavailable; no transport was started.\n",stderr);
        return 77;
    }
    UmiAiRequest *request=calloc(1U,sizeof *request);CHECK(request!=NULL);
    strcpy(request->request_id,"delta.bounds");strcpy(request->model_id,"test-model");
    request->message_count=1;request->max_output_tokens=64;
    request->messages[0].role=UMI_AI_ROLE_USER;strcpy(request->messages[0].text,"Quoted \"text\"\n");
    AwLocalChatDelta delta,before;memset(&delta,0xa5,sizeof delta);memcpy(&before,&delta,sizeof before);
    const char *mode=argv[1];char *encoded=NULL;int result=0;
    if(strcmp(mode,"done")==0){
        CHECK(AwLocalChatDeltaDecode(request,"[DONE]",6U,&delta)==UMI_STATUS_OK);
        CHECK(delta.done && !delta.has_finish && !delta.has_content && !delta.has_usage);
        CHECK(delta.text[0]=='\0' && delta.model_id[0]=='\0' && delta.completion_id[0]=='\0');
    }else if(strcmp(mode,"arguments")==0){
        CHECK(AwLocalChatDeltaDecode(NULL,"[DONE]",6U,&delta)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(AwLocalChatDeltaDecode(request,NULL,6U,&delta)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(AwLocalChatDeltaDecode(request,"[DONE]",0U,&delta)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(AwLocalChatDeltaDecode(request,"[DONE]",6U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        request->allow_tools=true;
        CHECK(AwLocalChatDeltaDecode(request,"[DONE]",6U,&delta)==UMI_STATUS_INVALID_ARGUMENT);
        request->allow_tools=false;memset(request->model_id,'x',sizeof request->model_id);
        CHECK(AwLocalChatDeltaDecode(request,"[DONE]",6U,&delta)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&delta,&before,sizeof delta)==0);
    }else if(strcmp(mode,"body-limit")==0){
        CHECK(AwLocalChatDeltaDecode(request,"x",AW_HTTP_BODY_CAPACITY,&delta)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&delta,&before,sizeof delta)==0);
    }else if(strcmp(mode,"padded-done")==0 || strcmp(mode,"raw-nul")==0 || strcmp(mode,"invalid-utf8")==0){
        const char *wire=strcmp(mode,"raw-nul")==0?"[DONE]\0":strcmp(mode,"invalid-utf8")==0?"\xff":"[DONE] ";
        size_t length=strcmp(mode,"invalid-utf8")==0?1U:7U;
        CHECK(AwLocalChatDeltaDecode(request,wire,length,&delta)==UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&delta,&before,sizeof delta)==0);
    }else if(strcmp(mode,"content-limit")==0){
        char wire[4096];
        const char *prefix="{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"";
        const char *suffix="\"},\"finish_reason\":null}]}";
        size_t offset=strlen(prefix);memcpy(wire,prefix,offset);
        memset(wire+offset,'a',UMI_AI_TEXT_CAPACITY);offset+=UMI_AI_TEXT_CAPACITY;
        memcpy(wire+offset,suffix,strlen(suffix)+1U);
        CHECK(AwLocalChatDeltaDecode(request,wire,strlen(wire),&delta)==UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&delta,&before,sizeof delta)==0);
    }else if(strncmp(mode,"encode-",7U)==0){
        UmiStatus wanted=UMI_STATUS_OK;
        if(strcmp(mode,"encode-tools")==0){request->allow_tools=true;wanted=UMI_STATUS_INVALID_ARGUMENT;}
        else if(strcmp(mode,"encode-role")==0){request->messages[0].role=UMI_AI_ROLE_TOOL;wanted=UMI_STATUS_PERMISSION_DENIED;}
        else if(strcmp(mode,"encode-count")==0){request->message_count=UMI_AI_MAX_MESSAGES+1U;wanted=UMI_STATUS_INVALID_ARGUMENT;}
        else if(strcmp(mode,"encode-temperature")==0){request->temperature=NAN;wanted=UMI_STATUS_INVALID_ARGUMENT;}
        else if(strcmp(mode,"encode-stream")!=0 && strcmp(mode,"encode-full")!=0){free(request);return 2;}
        UmiStatus status=strcmp(mode,"encode-full")==0?AwLocalChatEncode(request,&encoded):AwLocalChatEncodeStreaming(request,&encoded);
        CHECK(status==wanted);
        if(status==UMI_STATUS_OK){
            CHECK(encoded!=NULL);
            CHECK(strstr(encoded,strcmp(mode,"encode-full")==0?"\"stream\":false":"\"stream\":true")!=NULL);
            CHECK(strstr(encoded,"\\\"text\\\"\\n")!=NULL && strstr(encoded,"\"tools\"")==NULL);
        }else CHECK(encoded==NULL);
    }else result=2;
    AwLocalChatFree(encoded);free(request);return result;
}
