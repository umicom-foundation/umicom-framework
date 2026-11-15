/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_response_stream.c
 *
 * PURPOSE:
 *   Verify request identity, UTF-8 transactions, terminal states and owned model text copies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/response_stream.h"
#include "umicom/base/utf8.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
static const UmiAiResponseStreamIdentity key={11U,27U};
static int Expect(UmiAiResponseStream *stream,UmiAiResponseStreamState state,
    UmiStatus status,uint64_t last,uint64_t terminal,const char *expected,size_t pending)
{
    char *text=NULL;UmiAiResponseStreamSnapshot snapshot;
    CHECK(UmiAiResponseStreamCopy(stream,&text,&snapshot)==UMI_STATUS_OK);
    int valid=snapshot.identity.request_id==key.request_id &&
        snapshot.identity.model_revision==key.model_revision &&
        snapshot.state==state && snapshot.status==status &&
        snapshot.last_sequence==last && snapshot.terminal_sequence==terminal &&
        snapshot.text_bytes==strlen(expected) && snapshot.pending_bytes==pending &&
        snapshot.accepted_bytes==snapshot.text_bytes+pending &&
        strcmp(text,expected)==0 && UmiUtf8Validate((const unsigned char *)text,strlen(text),NULL);
    UmiAiResponseStreamTextFree(text);CHECK(valid);return 0;
}
static int Same(const UmiAiResponseStreamSnapshot *a,const UmiAiResponseStreamSnapshot *b)
{
    return a->identity.request_id==b->identity.request_id &&
        a->identity.model_revision==b->identity.model_revision &&
        a->state==b->state && a->status==b->status &&
        a->last_sequence==b->last_sequence && a->terminal_sequence==b->terminal_sequence &&
        a->accepted_bytes==b->accepted_bytes && a->text_bytes==b->text_bytes &&
        a->pending_bytes==b->pending_bytes && a->maximum_bytes==b->maximum_bytes;
}

/* Byte cursors refer to the same retained owner. Exercise both complete and
 * pending Unicode boundaries without changing the earlier full-copy coverage. */
static int RangeExercise(const char *mode)
{
    UmiAiResponseStream *stream = NULL;
    CHECK(UmiAiResponseStreamCreate(key, 128U, &stream) == UMI_STATUS_OK);
    char sentinel[] = "preserved";
    char *text = sentinel;
    size_t copied = 77U;
    UmiAiResponseStreamSnapshot snapshot, before;
    memset(&snapshot, 0x5a, sizeof snapshot);
    memcpy(&before, &snapshot, sizeof before);
    if (strcmp(mode, "range-arguments") == 0) {
        CHECK(UmiAiResponseStreamCopyRange(NULL, 0U, 4U, &text, &copied, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamCopyRange(stream, 0U, 4U, NULL, &copied, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamCopyRange(stream, 0U, 4U, &text, NULL, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamCopyRange(stream, 0U, 4U, &text, &copied, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamCopyRange(stream, 0U, 0U, &text, &copied, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamCopyRange(stream, 0U, UMI_AI_RESPONSE_STREAM_MAX_BYTES + 1U, &text, &copied, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamCopyRange(stream, SIZE_MAX, 4U, &text, &copied, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(text == sentinel && copied == 77U && memcmp(&before, &snapshot, sizeof before) == 0);
    } else if (strcmp(mode, "range-ascii") == 0) {
        CHECK(UmiAiResponseStreamPush(stream, key, 1U, (const unsigned char *)"abcdef", 6U, 1) == UMI_STATUS_OK);
        char assembled[7] = {0};
        size_t offset = 0U;
        for (unsigned index = 0U; index < 3U; ++index) {
            CHECK(UmiAiResponseStreamCopyRange(stream, offset, 2U, &text, &copied, &snapshot) == UMI_STATUS_OK);
            CHECK(copied == 2U && strlen(text) == copied && snapshot.text_bytes == 6U);
            memcpy(assembled + offset, text, copied); offset += copied;
            UmiAiResponseStreamTextFree(text);
        }
        CHECK(strcmp(assembled, "abcdef") == 0);
        CHECK(UmiAiResponseStreamCopyRange(stream, offset, 2U, &text, &copied, &snapshot) == UMI_STATUS_OK);
        CHECK(copied == 0U && text[0] == '\0' && snapshot.state == UMI_AI_RESPONSE_STREAM_COMPLETE);
        UmiAiResponseStreamTextFree(text);
    } else if (strcmp(mode, "range-utf8") == 0) {
        const unsigned char value[] = "A\xc2\xa2\xe2\x82\xac\xf0\x9f\x92\xa1Z";
        CHECK(UmiAiResponseStreamPush(stream, key, 1U, value, sizeof value - 1U, 1) == UMI_STATUS_OK);
        char assembled[sizeof value]; size_t offset = 0U;
        while (offset < sizeof value - 1U) {
            CHECK(UmiAiResponseStreamCopyRange(stream, offset, 4U, &text, &copied, &snapshot) == UMI_STATUS_OK);
            CHECK(copied > 0U && copied <= 4U && strlen(text) == copied &&
                UmiUtf8Validate((const unsigned char *)text, copied, NULL));
            memcpy(assembled + offset, text, copied); offset += copied;
            UmiAiResponseStreamTextFree(text);
        }
        assembled[offset] = '\0';
        CHECK(memcmp(assembled, value, sizeof value) == 0);
    } else if (strcmp(mode, "range-interior") == 0 || strcmp(mode, "range-budget") == 0) {
        const unsigned char value[] = "\xf0\x9f\x92\xa1";
        CHECK(UmiAiResponseStreamPush(stream, key, 1U, value, 4U, 1) == UMI_STATUS_OK);
        for (size_t index = 1U; index <= 3U; ++index) {
            UmiStatus expected = strcmp(mode, "range-interior") == 0 ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_CAPACITY_EXCEEDED;
            CHECK(UmiAiResponseStreamCopyRange(stream,
                expected == UMI_STATUS_INVALID_ARGUMENT ? index : 0U,
                expected == UMI_STATUS_INVALID_ARGUMENT ? 4U : index,
                &text, &copied, &snapshot) == expected);
            CHECK(text == sentinel && copied == 77U && memcmp(&before, &snapshot, sizeof before) == 0);
        }
        CHECK(UmiAiResponseStreamCopyRange(stream, 0U, 4U, &text, &copied, &snapshot) == UMI_STATUS_OK);
        CHECK(copied == 4U && memcmp(text, value, sizeof value) == 0);
        UmiAiResponseStreamTextFree(text);
    } else if (strcmp(mode, "range-pending") == 0) {
        const unsigned char prefix[] = {'A', 0xf0U, 0x9fU};
        const unsigned char suffix[] = {0x92U, 0xa1U};
        CHECK(UmiAiResponseStreamPush(stream, key, 1U, prefix, sizeof prefix, 0) == UMI_STATUS_OK);
        CHECK(UmiAiResponseStreamCopyRange(stream, 0U, 4U, &text, &copied, &snapshot) == UMI_STATUS_OK);
        CHECK(copied == 1U && strcmp(text, "A") == 0 && snapshot.pending_bytes == 2U);
        UmiAiResponseStreamTextFree(text);
        CHECK(UmiAiResponseStreamCopyRange(stream, 1U, 4U, &text, &copied, &snapshot) == UMI_STATUS_OK);
        CHECK(copied == 0U && text[0] == '\0' && snapshot.state == UMI_AI_RESPONSE_STREAM_ACTIVE);
        UmiAiResponseStreamTextFree(text);
        CHECK(UmiAiResponseStreamPush(stream, key, 2U, suffix, sizeof suffix, 1) == UMI_STATUS_OK);
        CHECK(UmiAiResponseStreamCopyRange(stream, 1U, 4U, &text, &copied, &snapshot) == UMI_STATUS_OK);
        CHECK(copied == 4U && strcmp(text, "\xf0\x9f\x92\xa1") == 0 && snapshot.pending_bytes == 0U);
        UmiAiResponseStreamTextFree(text);
    } else if (strcmp(mode, "range-cancelled") == 0 || strcmp(mode, "range-failed") == 0) {
        CHECK(UmiAiResponseStreamPush(stream, key, 1U, (const unsigned char *)"draft", 5U, 0) == UMI_STATUS_OK);
        UmiAiResponseStreamState expected = strcmp(mode, "range-cancelled") == 0 ?
            UMI_AI_RESPONSE_STREAM_CANCELLED : UMI_AI_RESPONSE_STREAM_FAILED;
        CHECK((expected == UMI_AI_RESPONSE_STREAM_CANCELLED ?
            UmiAiResponseStreamCancel(stream, key) : UmiAiResponseStreamFail(stream, key, 2U, UMI_STATUS_IO_ERROR)) == UMI_STATUS_OK);
        CHECK(UmiAiResponseStreamCopyRange(stream, 2U, 3U, &text, &copied, &snapshot) == UMI_STATUS_OK);
        CHECK(copied == 3U && strcmp(text, "aft") == 0 && snapshot.state == expected);
        UmiAiResponseStreamTextFree(text);
    } else if (strcmp(mode, "range-owned") == 0) {
        CHECK(UmiAiResponseStreamPush(stream, key, 1U, (const unsigned char *)"copy", 4U, 0) == UMI_STATUS_OK);
        CHECK(UmiAiResponseStreamCopyRange(stream, 1U, 2U, &text, &copied, &snapshot) == UMI_STATUS_OK);
        CHECK(UmiAiResponseStreamPush(stream, key, 2U, (const unsigned char *)"more", 4U, 1) == UMI_STATUS_OK);
        UmiAiResponseStreamDestroy(stream); stream = NULL;
        CHECK(copied == 2U && strcmp(text, "op") == 0 && snapshot.text_bytes == 4U);
        UmiAiResponseStreamTextFree(text);
    } else if (strcmp(mode, "range-empty") == 0) {
        CHECK(UmiAiResponseStreamCopyRange(stream, 0U, 4U, &text, &copied, &snapshot) == UMI_STATUS_OK);
        CHECK(copied == 0U && text[0] == '\0' && snapshot.text_bytes == 0U);
        UmiAiResponseStreamTextFree(text);
    } else { UmiAiResponseStreamDestroy(stream); return 2; }
    UmiAiResponseStreamDestroy(stream);
    return 0;
}

int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(strncmp(argv[1],"range-",6U)==0)return RangeExercise(argv[1]);
    const char *mode=argv[1];UmiAiResponseStream *stream=NULL;
    UmiAiResponseStreamSnapshot before,after;
    if(strcmp(mode,"arguments")==0){
        UmiAiResponseStream *sentinel=(UmiAiResponseStream *)(uintptr_t)1U;
        CHECK(UmiAiResponseStreamCreate((UmiAiResponseStreamIdentity){0U,1U},16U,&sentinel)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamCreate((UmiAiResponseStreamIdentity){1U,0U},16U,&sentinel)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamCreate(key,0U,&sentinel)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamCreate(key,UMI_AI_RESPONSE_STREAM_MAX_BYTES+1U,&sentinel)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(sentinel==(UmiAiResponseStream *)(uintptr_t)1U);
        CHECK(UmiAiResponseStreamCreate(key,16U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamPush(NULL,key,1U,NULL,0U,1)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamCancel(NULL,key)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamFail(NULL,key,1U,UMI_STATUS_IO_ERROR)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamInspect(NULL,&before)==UMI_STATUS_INVALID_ARGUMENT);
        UmiAiResponseStreamDestroy(NULL);UmiAiResponseStreamTextFree(NULL);
    }
    size_t budget=strcmp(mode,"capacity")==0 || strcmp(mode,"exact")==0?3U:128U;
    CHECK(UmiAiResponseStreamCreate(key,budget,&stream)==UMI_STATUS_OK);
    CHECK(UmiAiResponseStreamInspect(stream,&before)==UMI_STATUS_OK && before.maximum_bytes==budget);
    if(strcmp(mode,"arguments")==0){
        CHECK(UmiAiResponseStreamInspect(stream,&before)==UMI_STATUS_OK);
        CHECK(UmiAiResponseStreamPush(stream,key,1U,NULL,1U,0)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamPush(stream,key,1U,NULL,0U,2)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamInspect(stream,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamCopy(stream,NULL,&after)==UMI_STATUS_INVALID_ARGUMENT);
        char *text=NULL;
        CHECK(UmiAiResponseStreamCopy(stream,&text,NULL)==UMI_STATUS_INVALID_ARGUMENT && text==NULL);
        CHECK(UmiAiResponseStreamFail(stream,key,1U,UMI_STATUS_OK)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamFail(stream,key,1U,UMI_STATUS_CANCELLED)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamFail(stream,key,1U,(UmiStatus)999)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiResponseStreamInspect(stream,&after)==UMI_STATUS_OK && Same(&before,&after));
    }else if(strcmp(mode,"empty-final")==0){
        CHECK(UmiAiResponseStreamPush(stream,key,1U,NULL,0U,1)==UMI_STATUS_OK);
        CHECK(Expect(stream,UMI_AI_RESPONSE_STREAM_COMPLETE,UMI_STATUS_OK,1U,1U,"",0U)==0);
    }else if(strcmp(mode,"empty-event")==0){
        CHECK(UmiAiResponseStreamPush(stream,key,1U,NULL,0U,0)==UMI_STATUS_OK);
        CHECK(Expect(stream,UMI_AI_RESPONSE_STREAM_ACTIVE,UMI_STATUS_OK,1U,0U,"",0U)==0);
        CHECK(UmiAiResponseStreamPush(stream,key,2U,NULL,0U,1)==UMI_STATUS_OK);
        CHECK(Expect(stream,UMI_AI_RESPONSE_STREAM_COMPLETE,UMI_STATUS_OK,2U,2U,"",0U)==0);
    }else if(strcmp(mode,"exact")==0){
        CHECK(UmiAiResponseStreamPush(stream,key,1U,(const unsigned char *)"abc",3U,1)==UMI_STATUS_OK);
        CHECK(Expect(stream,UMI_AI_RESPONSE_STREAM_COMPLETE,UMI_STATUS_OK,1U,1U,"abc",0U)==0);
    }else if(strncmp(mode,"utf8-",5U)==0 || strcmp(mode,"byte-by-byte")==0){
        const unsigned char *value=(const unsigned char *)
            (strcmp(mode,"utf8-two")==0?"\xc2\xa2":strcmp(mode,"utf8-three")==0?"\xe2\x82\xac":
             strcmp(mode,"utf8-four")==0?"\xf0\x9f\x92\xa1":"a\xc2\xa2\xe2\x82\xac\xf0\x9f\x92\xa1");
        size_t length=strlen((const char *)value);
        for(size_t i=0U;i<length;++i){
            CHECK(UmiAiResponseStreamPush(stream,key,(uint64_t)i+1U,value+i,1U,i+1U==length)==UMI_STATUS_OK);
            char *partial=NULL;
            CHECK(UmiAiResponseStreamCopy(stream,&partial,&after)==UMI_STATUS_OK);
            CHECK(UmiUtf8Validate((const unsigned char *)partial,after.text_bytes,NULL));
            CHECK(after.accepted_bytes==i+1U && after.pending_bytes<=3U);
            CHECK(memcmp(partial,value,after.text_bytes)==0);
            UmiAiResponseStreamTextFree(partial);
        }
        CHECK(Expect(stream,UMI_AI_RESPONSE_STREAM_COMPLETE,UMI_STATUS_OK,(uint64_t)length,(uint64_t)length,(const char *)value,0U)==0);
    }else{
        CHECK(UmiAiResponseStreamPush(stream,key,1U,(const unsigned char *)"A",1U,0)==UMI_STATUS_OK);
        CHECK(UmiAiResponseStreamInspect(stream,&before)==UMI_STATUS_OK);
        if(strcmp(mode,"ascii")==0){
            CHECK(UmiAiResponseStreamPush(stream,key,2U,(const unsigned char *)"BC",2U,1)==UMI_STATUS_OK);
            CHECK(Expect(stream,UMI_AI_RESPONSE_STREAM_COMPLETE,UMI_STATUS_OK,2U,2U,"ABC",0U)==0);
        }else if(strcmp(mode,"capacity")==0){
            CHECK(UmiAiResponseStreamPush(stream,key,2U,(const unsigned char *)"BCD",3U,0)==UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(Expect(stream,UMI_AI_RESPONSE_STREAM_FAILED,UMI_STATUS_CAPACITY_EXCEEDED,1U,2U,"A",0U)==0);
        }else if(strcmp(mode,"wrong-request")==0 || strcmp(mode,"wrong-revision")==0){
            UmiAiResponseStreamIdentity wrong=key;
            if(strcmp(mode,"wrong-request")==0)++wrong.request_id;else ++wrong.model_revision;
            CHECK(UmiAiResponseStreamPush(stream,wrong,2U,(const unsigned char *)"B",1U,1)==UMI_STATUS_NOT_FOUND);
            CHECK(UmiAiResponseStreamCancel(stream,wrong)==UMI_STATUS_NOT_FOUND);
            CHECK(UmiAiResponseStreamFail(stream,wrong,2U,UMI_STATUS_TIMEOUT)==UMI_STATUS_NOT_FOUND);
            CHECK(UmiAiResponseStreamInspect(stream,&after)==UMI_STATUS_OK && Same(&before,&after));
        }else if(strcmp(mode,"duplicate")==0 || strcmp(mode,"gap")==0 || strcmp(mode,"zero-sequence")==0){
            uint64_t sequence=strcmp(mode,"duplicate")==0?1U:strcmp(mode,"gap")==0?3U:0U;
            CHECK(UmiAiResponseStreamPush(stream,key,sequence,(const unsigned char *)"B",1U,1)==UMI_STATUS_INVALID_STATE);
            CHECK(UmiAiResponseStreamFail(stream,key,sequence,UMI_STATUS_IO_ERROR)==UMI_STATUS_INVALID_STATE);
            CHECK(UmiAiResponseStreamInspect(stream,&after)==UMI_STATUS_OK && Same(&before,&after));
        }else if(strcmp(mode,"duplicate-final")==0 || strcmp(mode,"after-final-cancel")==0){
            CHECK(UmiAiResponseStreamPush(stream,key,2U,NULL,0U,1)==UMI_STATUS_OK);
            if(strcmp(mode,"duplicate-final")==0)
                CHECK(UmiAiResponseStreamPush(stream,key,3U,(const unsigned char *)"B",1U,1)==UMI_STATUS_INVALID_STATE);
            else CHECK(UmiAiResponseStreamCancel(stream,key)==UMI_STATUS_INVALID_STATE);
            CHECK(Expect(stream,UMI_AI_RESPONSE_STREAM_COMPLETE,UMI_STATUS_OK,2U,2U,"A",0U)==0);
        }else if(strncmp(mode,"cancel",6U)==0){
            int pending=strcmp(mode,"cancel-pending")==0;
            if(pending)CHECK(UmiAiResponseStreamPush(stream,key,2U,(const unsigned char *)"\xe2",1U,0)==UMI_STATUS_OK);
            CHECK(UmiAiResponseStreamCancel(stream,key)==UMI_STATUS_OK);
            if(strcmp(mode,"cancel-idempotent")==0)CHECK(UmiAiResponseStreamCancel(stream,key)==UMI_STATUS_OK);
            else CHECK(UmiAiResponseStreamPush(stream,key,pending?3U:2U,(const unsigned char *)"B",1U,1)==UMI_STATUS_INVALID_STATE);
            CHECK(Expect(stream,UMI_AI_RESPONSE_STREAM_CANCELLED,UMI_STATUS_CANCELLED,pending?2U:1U,0U,"A",pending?1U:0U)==0);
        }else if(strncmp(mode,"failure",7U)==0 || strcmp(mode,"late-after-failure")==0){
            int pending=strcmp(mode,"failure-pending")==0;
            if(pending)CHECK(UmiAiResponseStreamPush(stream,key,2U,(const unsigned char *)"\xe2",1U,0)==UMI_STATUS_OK);
            CHECK(UmiAiResponseStreamFail(stream,key,pending?3U:2U,UMI_STATUS_TIMEOUT)==UMI_STATUS_OK);
            CHECK(UmiAiResponseStreamFail(stream,key,pending?3U:2U,UMI_STATUS_IO_ERROR)==UMI_STATUS_INVALID_STATE);
            if(strcmp(mode,"late-after-failure")==0){
                CHECK(UmiAiResponseStreamPush(stream,key,2U,(const unsigned char *)"B",1U,1)==UMI_STATUS_INVALID_STATE);
                CHECK(UmiAiResponseStreamCancel(stream,key)==UMI_STATUS_INVALID_STATE);
            }
            CHECK(Expect(stream,UMI_AI_RESPONSE_STREAM_FAILED,UMI_STATUS_TIMEOUT,pending?2U:1U,pending?3U:2U,"A",pending?1U:0U)==0);
        }else if(strcmp(mode,"owned-copy")==0){
            char *text=NULL;
            CHECK(UmiAiResponseStreamCopy(stream,&text,&after)==UMI_STATUS_OK);
            CHECK(UmiAiResponseStreamPush(stream,key,2U,(const unsigned char *)"B",1U,1)==UMI_STATUS_OK);
            UmiAiResponseStreamDestroy(stream);stream=NULL;
            CHECK(strcmp(text,"A")==0 && after.state==UMI_AI_RESPONSE_STREAM_ACTIVE);
            UmiAiResponseStreamTextFree(text);
        }else{
            const unsigned char *bad=NULL;size_t length=0U;int final=0;
            if(strcmp(mode,"invalid-leading")==0){bad=(const unsigned char *)"\xff";length=1U;}
            else if(strcmp(mode,"invalid-continuation")==0){bad=(const unsigned char *)"\xe2\x20";length=2U;}
            else if(strcmp(mode,"overlong")==0){bad=(const unsigned char *)"\xe0\x80";length=2U;}
            else if(strcmp(mode,"surrogate")==0){bad=(const unsigned char *)"\xed\xa0";length=2U;}
            else if(strcmp(mode,"out-of-range")==0){bad=(const unsigned char *)"\xf4\x90";length=2U;}
            else if(strcmp(mode,"nul")==0){bad=(const unsigned char *)"B\0C";length=3U;}
            else if(strcmp(mode,"truncated-final")==0){bad=(const unsigned char *)"\xe2\x82";length=2U;final=1;}
            else if(strcmp(mode,"whole-chunk-atomic")==0){bad=(const unsigned char *)"BC\xff";length=3U;}
            else return 2;
            CHECK(UmiAiResponseStreamPush(stream,key,2U,bad,length,final)==UMI_STATUS_PARSE_ERROR);
            CHECK(Expect(stream,UMI_AI_RESPONSE_STREAM_FAILED,UMI_STATUS_PARSE_ERROR,1U,2U,"A",0U)==0);
        }
    }
    UmiAiResponseStreamDestroy(stream);
    return 0;
}
