/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/model_formats/test_gguf_file_ownership.c
 *
 * PURPOSE:
 *   Exercise chunk boundaries, file identity and late I/O failure without publishing partial GGUF observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "gguf_fixture.h"
#include "umicom/platform/input_stream.h"
static const unsigned char *input;
static size_t length,chunk=7U,visits;
static bool zero_chunk,partial_failure,late_cancel;
static UmiStatus final_status=UMI_STATUS_OK;
static UmiCancellationToken *token;
static UmiStatus synthetic_visit(const char *path,uint64_t maximum,UmiInputChunkVisitor visitor,
    void *context,uint64_t *out_size)
{
    (void)path;++visits;*out_size=0U;
    if(length>maximum)return UMI_STATUS_CAPACITY_EXCEEDED;
    if(zero_chunk) {UmiStatus result=visitor(NULL,0U,context);if(result!=UMI_STATUS_OK)return result;}
    for(size_t offset=0U;offset<length;) {
        if(partial_failure && offset>=24U)return UMI_STATUS_IO_ERROR;
        size_t amount=length-offset;if(amount>chunk)amount=chunk;
        UmiStatus result=visitor(input+offset,amount,context);
        if(result!=UMI_STATUS_OK)return result;
        offset+=amount;
    }
    if(late_cancel)umi_cancellation_token_request(token);
    if(final_status==UMI_STATUS_OK)*out_size=length;
    return final_status;
}
UmiStatus UmiTestGgufFileInspect(const char *path,const UmiGgufFileIdentity *expected,
    const UmiCancellationToken *cancel,UmiGgufFileInspection *out);
/* Replace only file delivery. The real scanner and SHA-256 implementation run
 * against the same bytes; native checked-reader tests cover OS handle behavior. */
#define UmiInputFileVisit synthetic_visit
#define UmiGgufFileInspect UmiTestGgufFileInspect
#include "../../src/model_formats/gguf_file.c"
#undef UmiGgufFileInspect
#undef UmiInputFileVisit
#define MODE(name) (strcmp(mode,(name))==0)
static int exercise(const char *mode)
{
    GgufFixture *f=malloc(sizeof(*f));GGUF_CHECK(f!=NULL);fixture_model(f);
    input=f->bytes;length=f->size;
    GGUF_CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);
    UmiGgufFileInspection output;memset(&output,0xa5,sizeof(output));
    unsigned char before[sizeof(output)];memcpy(before,&output,sizeof(output));
    UmiGgufFileIdentity expected={0};expected.bytes=length;
    UmiSha256 hash;UmiSha256Init(&hash);
    GGUF_CHECK(UmiSha256Update(&hash,input,length)==UMI_STATUS_OK);
    GGUF_CHECK(UmiSha256Final(&hash,expected.sha256)==UMI_STATUS_OK);
    const UmiGgufFileIdentity *reviewed=NULL;
    UmiStatus wanted=UMI_STATUS_OK;
    if(MODE("one-byte"))chunk=1U;
    else if(MODE("whole"))chunk=length;
    else if(MODE("zero-chunk"))zero_chunk=true;
    else if(MODE("late-close")){final_status=UMI_STATUS_IO_ERROR;wanted=final_status;}
    else if(MODE("changed-file")){final_status=UMI_STATUS_BUSY;wanted=final_status;}
    else if(MODE("late-cancel")){late_cancel=true;wanted=UMI_STATUS_CANCELLED;}
    else if(MODE("partial-read")){partial_failure=true;wanted=UMI_STATUS_IO_ERROR;}
    else if(MODE("truncated")){--length;wanted=UMI_STATUS_PARSE_ERROR;}
    else if(MODE("expected"))reviewed=&expected;
    else if(MODE("wrong-digest")){expected.sha256[0]^=1U;reviewed=&expected;wanted=UMI_STATUS_INVALID_STATE;}
    else if(MODE("expected-smaller")){--expected.bytes;reviewed=&expected;wanted=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(MODE("expected-larger")){++expected.bytes;reviewed=&expected;wanted=UMI_STATUS_INVALID_STATE;}
    else if(MODE("invalid-expected")){expected.bytes=23U;reviewed=&expected;wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(MODE("null-path") || MODE("null-output"))wanted=UMI_STATUS_INVALID_ARGUMENT;
    else if(MODE("pre-cancel")){umi_cancellation_token_request(token);wanted=UMI_STATUS_CANCELLED;}
    else if(MODE("payload-change")){fixture_bytes(f,"payload",7U);length=f->size;expected.bytes=length;reviewed=&expected;wanted=UMI_STATUS_INVALID_STATE;}
    else if(!MODE("split-fields") && !MODE("borrowed-expected")) {free(f);umi_cancellation_token_destroy(token);return 2;}
    GGUF_CHECK(UmiTestGgufFileInspect(MODE("null-path")?NULL:"synthetic",reviewed,token,MODE("null-output")?NULL:&output)==wanted);
    if(wanted==UMI_STATUS_OK) {
        GGUF_CHECK(output.file.bytes==length && output.metadata.role==UMI_GGUF_ROLE_MODEL_CANDIDATE);
        GGUF_CHECK(memcmp(output.file.sha256,expected.sha256,UMI_SHA256_BYTES)==0);
        if(MODE("borrowed-expected")) GGUF_CHECK(UmiTestGgufFileInspect("synthetic",&output.file,token,&output)==UMI_STATUS_OK);
    } else GGUF_CHECK(memcmp(&output,before,sizeof(output))==0);
    if(MODE("invalid-expected") || MODE("null-path") || MODE("null-output") || MODE("pre-cancel"))GGUF_CHECK(visits==0U);
    else GGUF_CHECK(visits==(MODE("borrowed-expected")?2U:1U));
    umi_cancellation_token_destroy(token);free(f);return 0;
}
int main(int argc,char **argv) { return argc==2?exercise(argv[1]):2; }
