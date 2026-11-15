/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_chat_stream_boundaries.c
 *
 * PURPOSE:
 *   Verify cancellation, late transport failure, exact budgets and unchanged outputs for stateful local streams.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_chat_stream_fixture.h"
static int Arguments(void)
{
    AwLocalChatStream *stream=NULL,*before=(AwLocalChatStream *)(uintptr_t)1U;
    UmiAiResponseStreamIdentity identity={17U,29U};
    CHECK(AwLocalChatStreamCreate(NULL,identity,64U,1024U,&before)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(before==(AwLocalChatStream *)(uintptr_t)1U);
    CHECK(CreateStream(0U,1024U,&before)==UMI_STATUS_INVALID_ARGUMENT && before==(AwLocalChatStream *)(uintptr_t)1U);
    CHECK(CreateStream(64U,0U,&before)==UMI_STATUS_INVALID_ARGUMENT && before==(AwLocalChatStream *)(uintptr_t)1U);
    CHECK(CreateStream(64U,16384U,&stream)==UMI_STATUS_OK);
    CHECK(AwLocalChatStreamFeed(stream,NULL,1U)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatStreamFeed(stream,NULL,0U)==UMI_STATUS_OK);
    CHECK(AwLocalChatStreamFeed(NULL,DONE,strlen(DONE))==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatStreamFinish(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatStreamCancel(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatStreamFail(stream,UMI_STATUS_OK)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatStreamFail(stream,UMI_STATUS_BUSY)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatStreamFail(stream,UMI_STATUS_CANCELLED)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatStreamFail(stream,(UmiStatus)999)==UMI_STATUS_INVALID_ARGUMENT);
    char *text=(char *)(uintptr_t)1U;AwLocalChatStreamSnapshot snapshot,old;
    memset(&snapshot,0xa5,sizeof snapshot);memcpy(&old,&snapshot,sizeof old);
    CHECK(AwLocalChatStreamCopy(NULL,&text,&snapshot)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatStreamCopy(stream,NULL,&snapshot)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatStreamCopy(stream,&text,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(text==(char *)(uintptr_t)1U && memcmp(&snapshot,&old,sizeof snapshot)==0);
    CHECK(CheckSnapshot(stream,AW_LOCAL_CHAT_ACTIVE,UMI_AI_RESPONSE_STREAM_ACTIVE,"",UMI_STATUS_OK)==0);
    AwLocalChatStreamDestroy(stream);AwLocalChatStreamDestroy(NULL);return 0;
}
static int CancelOrFail(bool after_marker,bool cancel)
{
    AwLocalChatStream *stream=NULL;CHECK(CreateStream(64U,16384U,&stream)==UMI_STATUS_OK);
    CHECK(AwLocalChatStreamFeed(stream,TEXT_HELLO,strlen(TEXT_HELLO))==UMI_STATUS_OK);
    if(after_marker)CHECK(AwLocalChatStreamFeed(stream,STOP DONE,strlen(STOP DONE))==UMI_STATUS_OK);
    UmiStatus expected=cancel?UMI_STATUS_CANCELLED:UMI_STATUS_IO_ERROR;
    if(cancel){
        CHECK(AwLocalChatStreamCancel(stream)==UMI_STATUS_OK);
        CHECK(AwLocalChatStreamCancel(stream)==UMI_STATUS_OK);
    }else CHECK(AwLocalChatStreamFail(stream,expected)==expected);
    CHECK(CheckSnapshot(stream,cancel?AW_LOCAL_CHAT_CANCELLED:AW_LOCAL_CHAT_FAILED,
        cancel?UMI_AI_RESPONSE_STREAM_CANCELLED:UMI_AI_RESPONSE_STREAM_FAILED,"Hello",expected)==0);
    CHECK(AwLocalChatStreamFeed(stream,DONE,strlen(DONE))==expected);
    CHECK(AwLocalChatStreamFinish(stream)==expected);
    CHECK(AwLocalChatStreamFail(stream,UMI_STATUS_TIMEOUT)==expected);
    AwLocalChatStreamDestroy(stream);return 0;
}
static int Limit(bool wire,bool exact)
{
    const char *all=TEXT_HELLO STOP DONE;
    size_t bytes=strlen(all);
    AwLocalChatStream *stream=NULL;
    CHECK(CreateStream(wire?64U:(exact?5U:4U),wire?(exact?bytes:bytes-1U):16384U,&stream)==UMI_STATUS_OK);
    UmiStatus status=AwLocalChatStreamFeed(stream,all,bytes);
    if(exact){
        CHECK(status==UMI_STATUS_OK && AwLocalChatStreamFinish(stream)==UMI_STATUS_OK);
        CHECK(CheckSnapshot(stream,AW_LOCAL_CHAT_COMPLETE,UMI_AI_RESPONSE_STREAM_COMPLETE,"Hello",UMI_STATUS_OK)==0);
    }else{
        CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(CheckSnapshot(stream,AW_LOCAL_CHAT_FAILED,UMI_AI_RESPONSE_STREAM_FAILED,wire?"Hello":"",status)==0);
    }
    AwLocalChatStreamDestroy(stream);return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!UmiAiWorkspaceLocalProviderAvailable()){fputs("SKIP: native local JSON capability unavailable.\n",stderr);return 77;}
    if(strcmp(argv[1],"arguments")==0)return Arguments();
    if(strcmp(argv[1],"cancel-active")==0)return CancelOrFail(false,true);
    if(strcmp(argv[1],"cancel-marker")==0)return CancelOrFail(true,true);
    if(strcmp(argv[1],"transport-active")==0)return CancelOrFail(false,false);
    if(strcmp(argv[1],"transport-marker")==0)return CancelOrFail(true,false);
    if(strcmp(argv[1],"text-limit")==0)return Limit(false,false);
    if(strcmp(argv[1],"text-exact")==0)return Limit(false,true);
    if(strcmp(argv[1],"wire-limit")==0)return Limit(true,false);
    if(strcmp(argv[1],"wire-exact")==0)return Limit(true,true);
    return 2;
}
