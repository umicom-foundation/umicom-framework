/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_chat_stream_faults.c
 *
 * PURPOSE:
 *   Verify committed-response ambiguity and separate terminal-publication failures at the stream composition boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_chat_stream_fixture.h"
static int fail_after_push,fail_after_final,fail_publication;
static uint64_t observed_failure_sequence;
UmiStatus __real_UmiAiResponseStreamPush(UmiAiResponseStream *,UmiAiResponseStreamIdentity,
    uint64_t,const unsigned char *,size_t,int);
UmiStatus __real_UmiAiResponseStreamFail(UmiAiResponseStream *,UmiAiResponseStreamIdentity,
    uint64_t,UmiStatus);
UmiStatus __wrap_UmiAiResponseStreamPush(UmiAiResponseStream *stream,UmiAiResponseStreamIdentity identity,
    uint64_t sequence,const unsigned char *bytes,size_t size,int final_chunk)
{
    UmiStatus status=__real_UmiAiResponseStreamPush(stream,identity,sequence,bytes,size,final_chunk);
    /* Simulate an unlock error after the real owner already committed its
     * mutation. This checks the documented ambiguity without corrupting a mutex. */
    if(status==UMI_STATUS_OK && ((fail_after_push && !final_chunk) || (fail_after_final && final_chunk)))
        return UMI_STATUS_INTERNAL_ERROR;
    return status;
}
UmiStatus __wrap_UmiAiResponseStreamFail(UmiAiResponseStream *stream,UmiAiResponseStreamIdentity identity,
    uint64_t sequence,UmiStatus status)
{
    observed_failure_sequence=sequence;
    if(fail_publication)return UMI_STATUS_IO_ERROR;
    return __real_UmiAiResponseStreamFail(stream,identity,sequence,status);
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!UmiAiWorkspaceLocalProviderAvailable()){fputs("SKIP: native local JSON capability unavailable.\n",stderr);return 77;}
    if(strcmp(argv[1],"committed-push")==0)fail_after_push=1;
    else if(strcmp(argv[1],"committed-final")==0)fail_after_final=1;
    else if(strcmp(argv[1],"failure-publication")==0)fail_publication=1;
    else return 2;
    AwLocalChatStream *stream=NULL;CHECK(CreateStream(64U,16384U,&stream)==UMI_STATUS_OK);
    UmiStatus status=AwLocalChatStreamFeed(stream,TEXT_HELLO,strlen(TEXT_HELLO));
    if(fail_after_push)CHECK(status==UMI_STATUS_INTERNAL_ERROR);
    else{
        CHECK(status==UMI_STATUS_OK);
        if(fail_after_final){
            CHECK(AwLocalChatStreamFeed(stream,STOP DONE,strlen(STOP DONE))==UMI_STATUS_OK);
            CHECK(AwLocalChatStreamFinish(stream)==UMI_STATUS_INTERNAL_ERROR);
        }else CHECK(AwLocalChatStreamFail(stream,UMI_STATUS_TIMEOUT)==UMI_STATUS_TIMEOUT);
    }
    char *text=NULL;AwLocalChatStreamSnapshot snapshot;
    CHECK(AwLocalChatStreamCopy(stream,&text,&snapshot)==UMI_STATUS_OK);
    CHECK(strcmp(text,"Hello")==0 && snapshot.phase==AW_LOCAL_CHAT_FAILED);
    if(fail_after_push){
        CHECK(snapshot.text.state==UMI_AI_RESPONSE_STREAM_FAILED && snapshot.failure==UMI_STATUS_INTERNAL_ERROR);
        CHECK(observed_failure_sequence==2U && snapshot.terminal_publication_status==UMI_STATUS_OK);
    }else if(fail_after_final){
        CHECK(snapshot.text.state==UMI_AI_RESPONSE_STREAM_COMPLETE && snapshot.failure==UMI_STATUS_INTERNAL_ERROR);
        CHECK(snapshot.terminal_publication_status==UMI_STATUS_INVALID_STATE && observed_failure_sequence==0U);
    }else{
        CHECK(snapshot.text.state==UMI_AI_RESPONSE_STREAM_ACTIVE && snapshot.failure==UMI_STATUS_TIMEOUT);
        CHECK(snapshot.terminal_publication_status==UMI_STATUS_IO_ERROR && observed_failure_sequence==2U);
    }
    CHECK(AwLocalChatStreamFinish(stream)==snapshot.failure);
    UmiAiResponseStreamTextFree(text);AwLocalChatStreamDestroy(stream);return 0;
}
