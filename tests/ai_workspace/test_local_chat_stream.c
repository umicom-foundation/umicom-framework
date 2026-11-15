/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_chat_stream.c
 *
 * PURPOSE:
 *   Exercise terminal order, provider identity and every fragmented success boundary without contacting a model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_chat_stream_fixture.h"
typedef struct SequenceFixture {const char *name,*wire;UmiStatus status;int finish;} SequenceFixture;
static const SequenceFixture refused[]={
    {"early-done",DONE,UMI_STATUS_PARSE_ERROR,0},
    {"changed-id",CHUNK("local-2","test-model","{\"content\":\"bad\"}","null",""),UMI_STATUS_PARSE_ERROR,0},
    {"changed-model",CHUNK("local-1","other","{\"content\":\"bad\"}","null",""),UMI_STATUS_PARSE_ERROR,0},
    {"usage-before-finish",USAGE,UMI_STATUS_PARSE_ERROR,0},
    {"choice-usage-before-finish",CHUNK("local-1","test-model","{}", "null",USAGE_FIELDS),UMI_STATUS_PARSE_ERROR,0},
    {"duplicate-finish",STOP STOP,UMI_STATUS_PARSE_ERROR,0},
    {"text-after-finish",STOP TEXT_HELLO,UMI_STATUS_PARSE_ERROR,0},
    {"role-after-finish",STOP ROLE,UMI_STATUS_PARSE_ERROR,0},
    {"empty-choice-after-finish",STOP CHUNK("local-1","test-model","{}","null",""),UMI_STATUS_PARSE_ERROR,0},
    {"choice-usage-after-finish",STOP CHUNK("local-1","test-model","{}","null",USAGE_FIELDS),UMI_STATUS_PARSE_ERROR,0},
    {"duplicate-usage",STOP USAGE USAGE,UMI_STATUS_PARSE_ERROR,0},
    {"usage-after-inline-usage",CHUNK("local-1","test-model","{}","\"stop\"",USAGE_FIELDS) USAGE,UMI_STATUS_PARSE_ERROR,0},
    {"usage-changed-id",STOP "data: {\"id\":\"other\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[]" USAGE_FIELDS "}\n\n",UMI_STATUS_PARSE_ERROR,0},
    {"duplicate-done",STOP DONE DONE,UMI_STATUS_PARSE_ERROR,0},
    {"text-after-done",STOP DONE TEXT_HELLO,UMI_STATUS_PARSE_ERROR,0},
    {"partial-after-done",STOP DONE "d",UMI_STATUS_PARSE_ERROR,0},
    {"comment-after-done",STOP DONE ":comment\n",UMI_STATUS_PARSE_ERROR,0},
    {"unsupported-event","event: error\n" STOP,UMI_STATUS_UNAVAILABLE,0},
    {"tool-call",CHUNK("local-1","test-model","{\"tool_calls\":[]}","null",""),UMI_STATUS_PERMISSION_DENIED,0},
    {"malformed","data: {broken}\n\n",UMI_STATUS_PARSE_ERROR,0},
    {"invalid-utf8","data: \xff\n\n",UMI_STATUS_PARSE_ERROR,0},
    {"no-finish","",UMI_STATUS_PARSE_ERROR,1},
    {"no-done",STOP,UMI_STATUS_PARSE_ERROR,1},
    {"undelimited-done",STOP "data: [DONE]\n",UMI_STATUS_PARSE_ERROR,1}
};
static int Refused(const SequenceFixture *fixture)
{
    AwLocalChatStream *stream=NULL;CHECK(CreateStream(64U,16384U,&stream)==UMI_STATUS_OK);
    CHECK(AwLocalChatStreamFeed(stream,TEXT_HELLO,strlen(TEXT_HELLO))==UMI_STATUS_OK);
    UmiStatus status=AwLocalChatStreamFeed(stream,fixture->wire,strlen(fixture->wire));
    if(fixture->finish){CHECK(status==UMI_STATUS_OK);status=AwLocalChatStreamFinish(stream);}
    CHECK(status==fixture->status);
    CHECK(CheckSnapshot(stream,AW_LOCAL_CHAT_FAILED,UMI_AI_RESPONSE_STREAM_FAILED,"Hello",fixture->status)==0);
    CHECK(AwLocalChatStreamFeed(stream,DONE,strlen(DONE))==fixture->status);
    CHECK(AwLocalChatStreamFinish(stream)==fixture->status);
    CHECK(AwLocalChatStreamCancel(stream)==fixture->status);
    CHECK(AwLocalChatStreamFail(stream,UMI_STATUS_IO_ERROR)==fixture->status);
    AwLocalChatStreamDestroy(stream);return 0;
}
static int Successful(const char *wire,const char *expected,UmiAiFinishReason finish,bool usage,bool fragment)
{
    size_t length=strlen(wire);
    /* Every two-part boundary exercises split CR/LF, UTF-8, JSON and delimiters.
     * A separate byte feed ensures no assumption about minimum network reads. */
    size_t attempts=fragment?length+2U:1U;
    for(size_t cut=0U;cut<attempts;++cut){
        AwLocalChatStream *stream=NULL;CHECK(CreateStream(64U,16384U,&stream)==UMI_STATUS_OK);
        if(!fragment)CHECK(AwLocalChatStreamFeed(stream,wire,length)==UMI_STATUS_OK);
        else if(cut<=length){
            CHECK(AwLocalChatStreamFeed(stream,wire,cut)==UMI_STATUS_OK);
            CHECK(AwLocalChatStreamFeed(stream,wire+cut,length-cut)==UMI_STATUS_OK);
        }else for(size_t i=0U;i<length;++i)CHECK(AwLocalChatStreamFeed(stream,wire+i,1U)==UMI_STATUS_OK);
        CHECK(CheckSnapshot(stream,AW_LOCAL_CHAT_MARKER,UMI_AI_RESPONSE_STREAM_ACTIVE,expected,UMI_STATUS_OK)==0);
        CHECK(AwLocalChatStreamFinish(stream)==UMI_STATUS_OK);
        CHECK(AwLocalChatStreamFinish(stream)==UMI_STATUS_OK);
        CHECK(CheckSnapshot(stream,AW_LOCAL_CHAT_COMPLETE,UMI_AI_RESPONSE_STREAM_COMPLETE,expected,UMI_STATUS_OK)==0);
        char *text=NULL;AwLocalChatStreamSnapshot snapshot;
        CHECK(AwLocalChatStreamCopy(stream,&text,&snapshot)==UMI_STATUS_OK);
        CHECK(snapshot.finish_reason==finish && snapshot.has_usage==usage);
        CHECK(strcmp(snapshot.completion_id,"local-1")==0);
        CHECK(snapshot.text.terminal_sequence==snapshot.text.last_sequence);
        if(usage)CHECK(snapshot.usage.input_tokens==2U && snapshot.usage.output_tokens==3U && snapshot.usage.total_tokens==5U);
        CHECK(AwLocalChatStreamFeed(stream,NULL,0U)==UMI_STATUS_INVALID_STATE);
        CHECK(AwLocalChatStreamCancel(stream)==UMI_STATUS_INVALID_STATE);
        CHECK(AwLocalChatStreamFail(stream,UMI_STATUS_IO_ERROR)==UMI_STATUS_INVALID_STATE);
        AwLocalChatStreamDestroy(stream);
        CHECK(strcmp(text,expected)==0);UmiAiResponseStreamTextFree(text);
    }
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!UmiAiWorkspaceLocalProviderAvailable()){fputs("SKIP: native local JSON capability unavailable.\n",stderr);return 77;}
    for(size_t i=0U;i<sizeof refused/sizeof refused[0];++i)
        if(strcmp(argv[1],refused[i].name)==0)return Refused(&refused[i]);
    if(strcmp(argv[1],"fragmented")==0)return Successful(
        "\xef\xbb\xbf: authored fixture\r\n\r\n" ROLE TEXT_HELLO
        CHUNK("local-1","test-model","{\"content\":\" € 🌍\"}","null","")
        STOP USAGE DONE " \t\r\n","Hello € 🌍",UMI_AI_FINISH_STOP,true,true);
    if(strcmp(argv[1],"empty-answer")==0)return Successful(STOP DONE,"",UMI_AI_FINISH_STOP,false,false);
    if(strcmp(argv[1],"length-answer")==0)return Successful(TEXT_HELLO LENGTH DONE,"Hello",UMI_AI_FINISH_LENGTH,false,false);
    if(strcmp(argv[1],"inline-usage")==0)return Successful(
        CHUNK("local-1","test-model","{\"content\":\"Hello\"}","\"stop\"",USAGE_FIELDS) DONE,
        "Hello",UMI_AI_FINISH_STOP,true,false);
    if(strcmp(argv[1],"empty-content")==0)return Successful(
        CHUNK("local-1","test-model","{\"content\":\"\"}","null","") STOP DONE,
        "",UMI_AI_FINISH_STOP,false,false);
    return 2;
}
