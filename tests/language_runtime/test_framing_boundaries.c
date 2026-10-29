/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_framing_boundaries.c
 *
 * PURPOSE:
 *   Verify bounded byte framing, atomic refusal and independent coalesced-message delivery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language_runtime/framing.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#expression); exit(1); \
} } while (0)

static void Pop(UmiLanguageRuntimeFramer *framer, const char *expected)
{
    char *output = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    size_t length=SIZE_MAX;
    CHECK(output!=NULL);
    CHECK(umi_language_runtime_framer_status(framer)==UMI_STATUS_OK);
    CHECK(umi_language_runtime_framer_pop(framer,output,
        UMI_LANGUAGE_RUNTIME_JSON_CAPACITY,&length)==UMI_STATUS_OK);
    CHECK(length==strlen(expected) && strcmp(output,expected)==0);
    free(output);
}
static void FeedText(UmiLanguageRuntimeFramer *framer, const char *frame)
{ CHECK(umi_language_runtime_framer_feed(framer,frame,strlen(frame))==UMI_STATUS_OK); }
static void Encode(UmiLanguageRuntimeFramer *framer, const char *body)
{
    char *encoded=malloc(UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY);
    size_t length=0U;
    CHECK(encoded!=NULL);
    CHECK(umi_language_runtime_frame_encode(body,encoded,
        UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY,&length)==UMI_STATUS_OK);
    CHECK(umi_language_runtime_framer_feed(framer,encoded,length)==UMI_STATUS_OK);
    free(encoded);
}
static void Refuse(UmiLanguageRuntimeFramer *framer, const char *header)
{
    char output[16]="unchanged";
    size_t length=73U;
    CHECK(umi_language_runtime_framer_feed(framer,header,strlen(header))==UMI_STATUS_PARSE_ERROR);
    CHECK(umi_language_runtime_framer_status(framer)==UMI_STATUS_PARSE_ERROR);
    CHECK(!umi_language_runtime_framer_has_message(framer));
    CHECK(umi_language_runtime_framer_pop(framer,output,sizeof(output),&length)==UMI_STATUS_PARSE_ERROR);
    CHECK(strcmp(output,"unchanged")==0 && length==73U);
    CHECK(umi_language_runtime_framer_feed(framer,"{}",2U)==UMI_STATUS_PARSE_ERROR);
    umi_language_runtime_framer_init(framer);
    Encode(framer,"{}");
    Pop(framer,"{}");
}
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    UmiLanguageRuntimeFramer *framer=calloc(1U,sizeof(*framer));
    UmiLanguageRuntimeFramer *before=calloc(1U,sizeof(*before));
    CHECK(framer!=NULL && before!=NULL);
    umi_language_runtime_framer_init(framer);
    const char *mode=argv[1];
    const char *frame="Content-Length: 2\r\n\r\n{}";
    if(strcmp(mode,"arguments")==0) {
        char output[32]="unchanged";size_t length=73U;
        CHECK(umi_language_runtime_framer_status(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!umi_language_runtime_framer_has_message(NULL));
        CHECK(umi_language_runtime_framer_feed(NULL,"",0U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_language_runtime_framer_feed(framer,NULL,1U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_language_runtime_framer_feed(framer,NULL,0U)==UMI_STATUS_OK);
        CHECK(umi_language_runtime_framer_pop(NULL,output,sizeof(output),&length)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_language_runtime_framer_pop(framer,NULL,sizeof(output),&length)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_language_runtime_framer_pop(framer,output,0U,&length)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_language_runtime_framer_pop(framer,output,sizeof(output),NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_language_runtime_frame_encode(NULL,output,sizeof(output),&length)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_language_runtime_frame_encode("{}",NULL,sizeof(output),&length)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_language_runtime_frame_encode("{}",output,0U,&length)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_language_runtime_frame_encode("{}",output,sizeof(output),NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(output,"unchanged")==0 && length==73U);
    } else if(strcmp(mode,"huge-feed")==0) {
        FeedText(framer,"Content-");
        *before=*framer;
        /* SIZE_MAX is rejected by count alone, before reading this short input.
         * A wrapped length addition must never reach a memory-copy operation. */
        CHECK(umi_language_runtime_framer_feed(framer,"x",SIZE_MAX)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(framer,before,sizeof(*framer))==0);
    } else if(strcmp(mode,"full-buffer")==0) {
        char *bytes=malloc(UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY);
        CHECK(bytes!=NULL);memset(bytes,'X',UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY);
        CHECK(umi_language_runtime_framer_feed(framer,bytes,
            UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY-1U)==UMI_STATUS_OK);
        *before=*framer;
        CHECK(umi_language_runtime_framer_feed(framer,"x",1U)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(framer,before,sizeof(*framer))==0);
        CHECK(umi_language_runtime_framer_status(framer)==UMI_STATUS_NOT_FOUND);
        free(bytes);
    } else if(strcmp(mode,"invalid-state")==0) {
        char output[16]="unchanged";size_t length=73U;
        framer->length=SIZE_MAX;
        CHECK(umi_language_runtime_framer_status(framer)==UMI_STATUS_INVALID_STATE);
        CHECK(umi_language_runtime_framer_feed(framer,"x",1U)==UMI_STATUS_INVALID_STATE);
        CHECK(umi_language_runtime_framer_pop(framer,output,sizeof(output),&length)==UMI_STATUS_INVALID_STATE);
        CHECK(strcmp(output,"unchanged")==0 && length==73U);
        umi_language_runtime_framer_init(framer);
        framer->header_ready=1;framer->header_length=SIZE_MAX;
        CHECK(umi_language_runtime_framer_status(framer)==UMI_STATUS_INVALID_STATE);
    } else if(strcmp(mode,"fragmented")==0) {
        for(size_t i=0U;i<strlen(frame);++i) {
            CHECK(umi_language_runtime_framer_feed(framer,frame+i,1U)==UMI_STATUS_OK);
            CHECK(umi_language_runtime_framer_has_message(framer)==(i+1U==strlen(frame)));
        }
        Pop(framer,"{}");
        CHECK(umi_language_runtime_framer_status(framer)==UMI_STATUS_NOT_FOUND);
    } else if(strcmp(mode,"coalesced")==0) {
        FeedText(framer,"Content-Length: 2\r\n\r\n{}Content-Length: 2\r\n\r\n[]");
        Pop(framer,"{}");Pop(framer,"[]");
        CHECK(framer->length==0U && umi_language_runtime_framer_status(framer)==UMI_STATUS_NOT_FOUND);
    } else if(strcmp(mode,"valid-before-invalid")==0) {
        FeedText(framer,"Content-Length: 2\r\n\r\n{}Content-Length: -2\r\n\r\n[]");
        Pop(framer,"{}");
        CHECK(umi_language_runtime_framer_status(framer)==UMI_STATUS_PARSE_ERROR);
        char output[16]="unchanged";size_t length=73U;
        CHECK(umi_language_runtime_framer_pop(framer,output,sizeof(output),&length)==UMI_STATUS_PARSE_ERROR);
        CHECK(strcmp(output,"unchanged")==0 && length==73U);
    } else if(strcmp(mode,"valid-before-partial")==0) {
        FeedText(framer,"Content-Length: 2\r\n\r\n{}Content-Len");
        Pop(framer,"{}");
        CHECK(umi_language_runtime_framer_status(framer)==UMI_STATUS_NOT_FOUND);
        FeedText(framer,"gth: 2\r\n\r\n[]");Pop(framer,"[]");
    } else if(strcmp(mode,"output-capacity")==0) {
        FeedText(framer,frame);*before=*framer;
        char output[16]="unchanged";size_t length=73U;
        CHECK(umi_language_runtime_framer_pop(framer,output,2U,&length)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(framer,before,sizeof(*framer))==0);
        CHECK(strcmp(output,"unchanged")==0 && length==73U);
        CHECK(umi_language_runtime_framer_pop(framer,output,3U,&length)==UMI_STATUS_OK);
        CHECK(length==2U && strcmp(output,"{}")==0);
    } else if(strcmp(mode,"incomplete-output")==0) {
        FeedText(framer,"Content-Length: 2\r\n\r\n{");*before=*framer;
        char output[16]="unchanged";size_t length=73U;
        CHECK(umi_language_runtime_framer_pop(framer,output,sizeof(output),&length)==UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(framer,before,sizeof(*framer))==0);
        CHECK(strcmp(output,"unchanged")==0 && length==73U);
        FeedText(framer,"}");Pop(framer,"{}");
    } else if(strcmp(mode,"encode-capacity")==0) {
        char output[64]="unchanged";size_t length=73U;
        CHECK(umi_language_runtime_frame_encode("{}",output,5U,&length)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(output,"unchanged")==0 && length==73U);
        CHECK(umi_language_runtime_frame_encode("{}",output,strlen(frame),&length)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(output,"unchanged")==0 && length==73U);
        CHECK(umi_language_runtime_frame_encode("{}",output,strlen(frame)+1U,&length)==UMI_STATUS_OK);
        CHECK(length==strlen(frame) && strcmp(output,frame)==0);
    } else if(strcmp(mode,"encode-bounded")==0) {
        char *body=malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
        char output[64]="unchanged";size_t length=73U;
        CHECK(body!=NULL);memset(body,'x',UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
        CHECK(umi_language_runtime_frame_encode(body,output,sizeof(output),&length)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(output,"unchanged")==0 && length==73U);free(body);
    } else if(strcmp(mode,"encode-in-place")==0) {
        char output[64]="{}";size_t length=0U;
        CHECK(umi_language_runtime_frame_encode(output,output,sizeof(output),&length)==UMI_STATUS_OK);
        CHECK(length==strlen(frame) && strcmp(output,frame)==0);
    } else if(strcmp(mode,"utf8-bytes")==0) {
        const char *body="{\"text\":\"\xc3\xa9\"}";
        char output[128];size_t length=0U;
        CHECK(umi_language_runtime_frame_encode(body,output,sizeof(output),&length)==UMI_STATUS_OK);
        CHECK(strncmp(output,"Content-Length: 13\r\n",20U)==0);
        FeedText(framer,output);Pop(framer,body);
    } else if(strcmp(mode,"maximum-body")==0) {
        char *body=malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
        CHECK(body!=NULL);memset(body,' ',UMI_LANGUAGE_RUNTIME_JSON_CAPACITY-1U);
        body[0]='{';body[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY-2U]='}';
        body[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY-1U]='\0';
        Encode(framer,body);Pop(framer,body);free(body);
    } else if(strcmp(mode,"zero-body")==0) {
        FeedText(framer,"Content-Length: 0\r\n\r\n");Pop(framer,"");
    } else if(strcmp(mode,"ascii-case")==0) {
        FeedText(framer,"cOnTeNt-LeNgTh:\t002 \t\r\nX-Trace: local\r\n\r\n{}");Pop(framer,"{}");
    } else if(strcmp(mode,"charset-utf8")==0) {
        FeedText(framer,"Content-Type: application/vscode-jsonrpc; charset=UTF-8\r\nContent-Length: 2\r\n\r\n{}");Pop(framer,"{}");
    } else if(strcmp(mode,"charset-alias")==0) {
        FeedText(framer,"Content-Length: 2\r\nContent-Type: application/vscode-jsonrpc; charset=utf8\r\n\r\n{}");Pop(framer,"{}");
    } else if(strcmp(mode,"charset-quoted")==0) {
        FeedText(framer,"Content-Length: 2\r\nContent-Type: application/vscode-jsonrpc; x=local; CHARSET = \"utf-8\" \r\n\r\n{}");Pop(framer,"{}");
    } else if(strcmp(mode,"raw-nul")==0) {
        FeedText(framer,"Content-Length: 2\r\n\r\n");*before=*framer;
        const char body[2]={'{','\0'};
        CHECK(umi_language_runtime_framer_feed(framer,body,sizeof(body))==UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(framer,before,sizeof(*framer))==0);
        FeedText(framer,"{}");Pop(framer,"{}");
    } else {
        struct Invalid { const char *name,*frame; } invalid[]={
            {"duplicate-length","Content-Length: 2\r\nContent-Length: 2\r\n\r\n{}"},
            {"conflicting-length","Content-Length: 2\r\ncontent-length: 3\r\n\r\n{}"},
            {"signed-positive","Content-Length: +2\r\n\r\n{}"},
            {"signed-negative","Content-Length: -2\r\n\r\n{}"},
            {"overflow-length","Content-Length: 9999999999999999999999999999999999999999\r\n\r\n{}"},
            {"oversize-length","Content-Length: 65536\r\n\r\n{}"},
            {"empty-length","Content-Length: \r\n\r\n{}"},
            {"length-junk","Content-Length: 2x\r\n\r\n{}"},
            {"missing-length","X-Trace: local\r\n\r\n{}"},
            {"bad-name","Content Length: 2\r\n\r\n{}"},
            {"missing-colon","Content-Length 2\r\n\r\n{}"},
            {"non-ascii-header","Content-Length: 2\r\nX-Trace: \xc3\xa9\r\n\r\n{}"},
            {"control-header","Content-Length: 2\r\nX-Trace: \x01\r\n\r\n{}"},
            {"charset-other","Content-Length: 2\r\nContent-Type: application/vscode-jsonrpc; charset=utf-16\r\n\r\n{}"},
            {"charset-duplicate","Content-Length: 2\r\nContent-Type: application/vscode-jsonrpc; charset=utf8; charset=utf8\r\n\r\n{}"},
            {"charset-empty","Content-Length: 2\r\nContent-Type: application/vscode-jsonrpc; charset=\r\n\r\n{}"},
            {"duplicate-type","Content-Length: 2\r\nContent-Type: application/vscode-jsonrpc\r\nContent-Type: application/vscode-jsonrpc\r\n\r\n{}"}
        };
        int found=0;
        for(size_t i=0U;i<sizeof(invalid)/sizeof(invalid[0]);++i)
            if(strcmp(mode,invalid[i].name)==0) { Refuse(framer,invalid[i].frame);found=1;break; }
        if(!found) { free(before);free(framer);return 2; }
    }
    free(before);free(framer);
    return 0;
}
