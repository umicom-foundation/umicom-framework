/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/protocol_frames/main.c
 *
 * PURPOSE:
 *   Demonstrate fragmented input and independent delivery of coalesced protocol frames.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language_runtime/framing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int Expect(UmiStatus actual,UmiStatus expected,const char *step)
{
    if(actual==expected) return 1;
    fprintf(stderr,"%s: expected status %d, received %d\n",step,(int)expected,(int)actual);
    return 0;
}
int main(void)
{
    /* The bounded framer is larger than a small callback stack. One heap owner
     * keeps it alive across feeds and releases it on every exit path. */
    UmiLanguageRuntimeFramer *framer=malloc(sizeof(*framer));
    if(framer==NULL) { fputs("Cannot allocate a framer.\n",stderr);return 1; }
    umi_language_runtime_framer_init(framer);
    const char *json="{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":null}";
    char encoded[256],output[256];
    size_t encoded_length=0U,output_length=0U;
    int result=1;
    if(!Expect(umi_language_runtime_frame_encode(json,encoded,sizeof(encoded),&encoded_length),
        UMI_STATUS_OK,"encode")) goto finished;
    /* A pipe read could end anywhere. This split deliberately ends inside the
     * Content-Length field name, rather than at a convenient line boundary. */
    if(!Expect(umi_language_runtime_framer_feed(framer,encoded,7U),UMI_STATUS_OK,"first fragment"))
        goto finished;
    if(!Expect(umi_language_runtime_framer_status(framer),UMI_STATUS_NOT_FOUND,"incomplete header"))
        goto finished;
    puts("First fragment: more bytes are needed.");
    if(!Expect(umi_language_runtime_framer_feed(framer,encoded+7U,encoded_length-7U),
        UMI_STATUS_OK,"remaining fragment")) goto finished;
    if(!Expect(umi_language_runtime_framer_pop(framer,output,sizeof(output),&output_length),
        UMI_STATUS_OK,"complete message")) goto finished;
    if(output_length!=strlen(json) || strcmp(output,json)!=0) goto finished;
    printf("Complete body: %s\n",output);

    /* Both frames arrive in one read. A malformed second header must not make
     * the first valid message disappear. Reconnect/reset after framing errors. */
    char combined[512];
    const char *bad="Content-Length: -1\r\n\r\nx";
    memcpy(combined,encoded,encoded_length);
    memcpy(combined+encoded_length,bad,strlen(bad));
    if(!Expect(umi_language_runtime_framer_feed(framer,combined,encoded_length+strlen(bad)),
        UMI_STATUS_OK,"coalesced input")) goto finished;
    if(!Expect(umi_language_runtime_framer_pop(framer,output,sizeof(output),&output_length),
        UMI_STATUS_OK,"valid predecessor")) goto finished;
    if(output_length!=strlen(json) || strcmp(output,json)!=0) goto finished;
    puts("Coalesced input: the valid first body was delivered.");
    if(!Expect(umi_language_runtime_framer_status(framer),UMI_STATUS_PARSE_ERROR,
        "malformed successor")) goto finished;
    puts("Next header: malformed; no extra read is needed.");
    result=0;
finished:
    free(framer);
    return result;
}
