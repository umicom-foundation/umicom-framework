/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_chat_delta_offline.c
 *
 * PURPOSE:
 *   Verify unavailable optional JSON transport never becomes a simulated streaming success.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_chat_internal.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(void)
{
    char sentinel='x';char *encoded=&sentinel;
    CHECK(AwLocalChatEncodeStreaming(NULL,&encoded)==UMI_STATUS_UNAVAILABLE && encoded==NULL);
    CHECK(AwLocalChatEncodeStreaming(NULL,NULL)==UMI_STATUS_UNAVAILABLE);
    AwLocalChatDelta delta,before;memset(&delta,0xa5,sizeof delta);memcpy(&before,&delta,sizeof before);
    CHECK(AwLocalChatDeltaDecode(NULL,"[DONE]",6U,&delta)==UMI_STATUS_UNAVAILABLE);
    CHECK(memcmp(&delta,&before,sizeof delta)==0);
    CHECK(AwLocalChatDeltaDecode(NULL,NULL,0,NULL)==UMI_STATUS_UNAVAILABLE);
    AwLocalChatFree(NULL);return 0;
}
