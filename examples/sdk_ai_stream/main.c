/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/sdk_ai_stream/main.c
 *
 * PURPOSE:
 *   Demonstrate bounded request-owned model text through an installed native Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/response_stream.h"
#include <stdio.h>
#include <string.h>

/* Reassemble the retained fixture using small complete-scalar ranges from the
 * installed API. This also makes a missing new SDK symbol a link failure. */
static UmiStatus VerifyRanges(UmiAiResponseStream *stream, const char *expected,
    const UmiAiResponseStreamSnapshot *complete)
{
    size_t offset = 0U;
    for (;;) {
        char *part = NULL;
        size_t copied = 0U;
        UmiAiResponseStreamSnapshot observed;
        UmiStatus status = UmiAiResponseStreamCopyRange(stream, offset, 4U,
            &part, &copied, &observed);
        if (status != UMI_STATUS_OK) return status;
        int valid = copied <= 4U && offset + copied <= complete->text_bytes &&
            strlen(part) == copied && memcmp(part, expected + offset, copied) == 0 &&
            observed.identity.request_id == complete->identity.request_id &&
            observed.identity.model_revision == complete->identity.model_revision &&
            observed.state == UMI_AI_RESPONSE_STREAM_COMPLETE &&
            observed.text_bytes == complete->text_bytes;
        UmiAiResponseStreamTextFree(part);
        if (!valid) return UMI_STATUS_INTERNAL_ERROR;
        if (copied == 0U)
            return offset == complete->text_bytes ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
        offset += copied;
    }
}


/* Deterministic transport fixture: these bytes are authored text, not a model
 * response. A real provider adapter would supply decoded text events here. */
int main(void)
{
    UmiAiResponseStream *stream=NULL;
    const UmiAiResponseStreamIdentity identity={1U,7U};
    UmiStatus status=UmiAiResponseStreamCreate(identity,4096U,&stream);
    char *text=NULL;
    UmiAiResponseStreamSnapshot snapshot;
    if(status!=UMI_STATUS_OK)goto failed;
    status=UmiAiResponseStreamPush(stream,identity,1U,(const unsigned char *)"Native ",7U,0);
    if(status!=UMI_STATUS_OK)goto failed;
    const UmiAiResponseStreamIdentity retired={1U,6U};
    status=UmiAiResponseStreamPush(stream,retired,2U,(const unsigned char *)"old model",9U,0);
    if(status!=UMI_STATUS_NOT_FOUND){
        if(status==UMI_STATUS_OK)status=UMI_STATUS_INTERNAL_ERROR;
        goto failed;
    }
    status=UmiAiResponseStreamPush(stream,identity,2U,(const unsigned char *)"\xe2\x82",2U,0);
    if(status!=UMI_STATUS_OK)goto failed;
    status=UmiAiResponseStreamPush(stream,identity,3U,(const unsigned char *)"\xac",1U,0);
    if(status!=UMI_STATUS_OK)goto failed;
    status=UmiAiResponseStreamPush(stream,identity,4U,(const unsigned char *)" text.",6U,1);
    if(status!=UMI_STATUS_OK)goto failed;
    status=UmiAiResponseStreamCopy(stream,&text,&snapshot);
    if(status!=UMI_STATUS_OK)goto failed;
    status=VerifyRanges(stream,text,&snapshot);
    if(status!=UMI_STATUS_OK)goto failed;
    UmiAiResponseStreamDestroy(stream);stream=NULL;
    /* The detached copy survives its stream. This example starts no worker,
     * so no join is required; threaded callers must quiesce before Destroy. */
    int written=printf("Authored text fixture (no model inference):\n%s\n"
        "Request %llu, model revision %llu, accepted %zu bytes, state %d\n",
        text,(unsigned long long)snapshot.identity.request_id,
        (unsigned long long)snapshot.identity.model_revision,
        snapshot.accepted_bytes,(int)snapshot.state);
    UmiAiResponseStreamTextFree(text);
    if(written<0 || fflush(stdout)!=0 || ferror(stdout))return 3;
    return 0;
failed:
    fprintf(stderr,"Stream example failed: %s\n",umi_status_text(status));
    UmiAiResponseStreamTextFree(text);
    UmiAiResponseStreamDestroy(stream);
    return 2;
}
