/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_response_stream_sequence.c
 *
 * PURPOSE:
 *   Check sequence exhaustion without iterating through the entire uint64_t space.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* White-box boundary fixture: compile the real implementation once here and
 * seed only its otherwise unreachable counter. Public-path cases remain in
 * the separately linked normal fixture. No test hooks enter installed APIs. */
#include "../../src/ai/response_stream.c"
#include <stdio.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(void)
{
    UmiAiResponseStream *stream=NULL;
    const UmiAiResponseStreamIdentity key={1U,1U};
    CHECK(UmiAiResponseStreamCreate(key,8U,&stream)==UMI_STATUS_OK);
    stream->snapshot.last_sequence=UINT64_MAX-1U;
    CHECK(UmiAiResponseStreamPush(stream,key,UINT64_MAX,(const unsigned char *)"A",1U,0)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(stream->snapshot.state==UMI_AI_RESPONSE_STREAM_ACTIVE &&
        stream->snapshot.last_sequence==UINT64_MAX-1U && stream->snapshot.accepted_bytes==0U);
    CHECK(UmiAiResponseStreamPush(stream,key,UINT64_MAX,(const unsigned char *)"A",1U,1)==UMI_STATUS_OK);
    CHECK(stream->snapshot.last_sequence==UINT64_MAX && stream->snapshot.terminal_sequence==UINT64_MAX);
    CHECK(UmiAiResponseStreamPush(stream,key,0U,NULL,0U,1)==UMI_STATUS_INVALID_STATE);
    UmiAiResponseStreamDestroy(stream);
    stream=NULL;
    CHECK(UmiAiResponseStreamCreate(key,8U,&stream)==UMI_STATUS_OK);
    stream->snapshot.last_sequence=UINT64_MAX-1U;
    CHECK(UmiAiResponseStreamFail(stream,key,UINT64_MAX,UMI_STATUS_IO_ERROR)==UMI_STATUS_OK);
    CHECK(stream->snapshot.state==UMI_AI_RESPONSE_STREAM_FAILED &&
        stream->snapshot.terminal_sequence==UINT64_MAX);
    UmiAiResponseStreamDestroy(stream);return 0;
}
