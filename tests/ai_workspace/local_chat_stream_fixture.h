/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/local_chat_stream_fixture.h
 *
 * PURPOSE:
 *   Share authored wire fixtures and ownership checks across local stream sequence regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_LOCAL_CHAT_STREAM_FIXTURE_H
#define UMICOM_TEST_LOCAL_CHAT_STREAM_FIXTURE_H
#include "local_chat_stream_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;} } while(0)
#define CHUNK(id,model,delta,finish,extra) "data: {\"id\":\"" id "\",\"object\":\"chat.completion.chunk\",\"model\":\"" model "\",\"choices\":[{\"index\":0,\"delta\":" delta ",\"finish_reason\":" finish "}]" extra "}\n\n"
#define TEXT_HELLO CHUNK("local-1","test-model","{\"content\":\"Hello\"}","null","")
#define ROLE CHUNK("local-1","test-model","{\"role\":\"assistant\"}","null","")
#define STOP CHUNK("local-1","test-model","{}","\"stop\"","")
#define LENGTH CHUNK("local-1","test-model","{}","\"length\"","")
#define USAGE_FIELDS ",\"usage\":{\"prompt_tokens\":2,\"completion_tokens\":3,\"total_tokens\":5}"
#define USAGE "data: {\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[]" USAGE_FIELDS "}\n\n"
#define DONE "data: [DONE]\n\n"
static inline UmiStatus CreateStream(size_t text_limit,uint64_t wire_limit,AwLocalChatStream **out)
{
    UmiAiRequest *request=calloc(1U,sizeof *request);
    if(request==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    strcpy(request->request_id,"sequence.test");strcpy(request->model_id,"test-model");
    UmiAiResponseStreamIdentity identity={17U,29U};
    UmiStatus status=AwLocalChatStreamCreate(request,identity,text_limit,wire_limit,out);
    /* The stream must retain only copied identity, not this request allocation. */
    memset(request,0xa5,sizeof *request);free(request);return status;
}
static inline int CheckSnapshot(AwLocalChatStream *stream,AwLocalChatStreamPhase phase,
    UmiAiResponseStreamState state,const char *expected,UmiStatus failure)
{
    char *text=NULL;AwLocalChatStreamSnapshot snapshot;
    CHECK(AwLocalChatStreamCopy(stream,&text,&snapshot)==UMI_STATUS_OK);
    CHECK(snapshot.phase==phase && snapshot.text.state==state);
    CHECK(snapshot.failure==failure && strcmp(text,expected)==0);
    CHECK(snapshot.text.identity.request_id==17U && snapshot.text.identity.model_revision==29U);
    CHECK(snapshot.text.text_bytes==strlen(expected) && snapshot.text.pending_bytes==0U);
    UmiAiResponseStreamTextFree(text);return 0;
}
#endif
