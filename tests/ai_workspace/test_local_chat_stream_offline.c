/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_chat_stream_offline.c
 *
 * PURPOSE:
 *   Confirm the real offline codec prevents creating a local protocol stream without native JSON capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_chat_stream_fixture.h"
#include "local_http_stream_internal.h"
int main(void)
{
    AwLocalChatStream *stream=(AwLocalChatStream *)(uintptr_t)1U;
    CHECK(CreateStream(64U,16384U,&stream)==UMI_STATUS_UNAVAILABLE);
    CHECK(stream==(AwLocalChatStream *)(uintptr_t)1U);
    AwLocalHttpStreamResult result,before;
    memset(&result,0xa5,sizeof result);memcpy(&before,&result,sizeof before);
    CHECK(AwLocalChatExchangeStream(8080U,1000U,"{}",16384U,NULL,stream,&result)==UMI_STATUS_UNAVAILABLE);
    CHECK(memcmp(&result,&before,sizeof result)==0);
    return 0;
}
