/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_run_inspection.c
 *
 * PURPOSE:
 *   Verify run-admission policy, one-pass delegation and preserved output without reading or loading real model weights.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/local_models/manager.h"
#include "umicom/base/sha256.h"
#ifdef UMICOM_TEST_RUN_INSPECTION_ENABLED
#define UMICOM_MODELS_HAS_TENSOR_INSPECTION 1
#include "umicom/model_formats/gguf.h"
static UmiGgufFileInspection fixture;
static UmiStatus read_status=UMI_STATUS_OK;
static unsigned reads;
static bool cancel_after_read;
static UmiStatus read_fixture(const char *path,const UmiGgufFileIdentity *expected,
    const UmiCancellationToken *cancel,UmiGgufFileInspection *out)
{
    assert(path!=NULL && strcmp(path,"fixture.gguf")==0 && expected==NULL);
    ++reads;
    if(read_status!=UMI_STATUS_OK) return read_status;
    *out=fixture;
    if(cancel_after_read) umi_cancellation_token_request((UmiCancellationToken *)cancel);
    return UMI_STATUS_OK;
}
#define UmiGgufFileInspect read_fixture
#else
#undef UMICOM_MODELS_HAS_TENSOR_INSPECTION
#endif
#define UmiModelInspectForRun UmiTestModelInspectForRun
#define UmiModelRunInspectionAvailable UmiTestModelRunInspectionAvailable
#include "../../src/local_models/run_inspection.c"
#undef UmiModelRunInspectionAvailable
#undef UmiModelInspectForRun
#ifdef UMICOM_TEST_RUN_INSPECTION_ENABLED
#undef UmiGgufFileInspect
#endif
#define MODE(name) (strcmp(argv[1],name)==0)
int main(int argc,char **argv)
{
    assert(argc==2);
    UmiModelInspection out,before;
    memset(&out,0x5a,sizeof(out));
    unsigned char hash[32];memset(hash,0xd1,sizeof(hash));
    char expected[65];UmiSha256Hex(hash,expected);
    UmiCancellationToken *token=NULL;
    assert(umi_cancellation_token_create(&token)==UMI_STATUS_OK && token!=NULL);
    const char *path="fixture.gguf",*reviewed=expected;
    UmiModelInspection *destination=&out;
    UmiStatus wanted=UMI_STATUS_OK;
#ifdef UMICOM_TEST_RUN_INSPECTION_ENABLED
    assert(UmiTestModelRunInspectionAvailable());
    fixture.file.bytes=128U;memcpy(fixture.file.sha256,hash,sizeof(hash));
    fixture.metadata.version=3U;fixture.metadata.tensors=1U;fixture.metadata.entries=1U;
    fixture.metadata.role=UMI_GGUF_ROLE_MODEL_CANDIDATE;
    if(MODE("borrowed-digest")) { memcpy(out.sha256,expected,sizeof(expected));reviewed=out.sha256; }
    else if(MODE("wrong-digest")) { expected[0]='0';wanted=UMI_STATUS_INVALID_STATE; }
    else if(MODE("auxiliary")) { fixture.metadata.role=UMI_GGUF_ROLE_AUXILIARY;wanted=UMI_STATUS_NOT_IMPLEMENTED; }
    else if(MODE("unknown")) { fixture.metadata.role=UMI_GGUF_ROLE_UNKNOWN;wanted=UMI_STATUS_NOT_IMPLEMENTED; }
    else if(MODE("vocabulary")) { fixture.metadata.role=UMI_GGUF_ROLE_VOCABULARY_ONLY;fixture.metadata.tensors=0U;wanted=UMI_STATUS_PARSE_ERROR; }
    else if(MODE("zero-tensors")) { fixture.metadata.tensors=0U;wanted=UMI_STATUS_PARSE_ERROR; }
    else if(MODE("tensor-bound")) { fixture.metadata.tensors=7U;wanted=UMI_STATUS_PARSE_ERROR; }
    else if(MODE("entry-bound")) { fixture.metadata.entries=9U;wanted=UMI_STATUS_PARSE_ERROR; }
    else if(MODE("short-file")) { fixture.file.bytes=23U;wanted=UMI_STATUS_PARSE_ERROR; }
    else if(MODE("oversized-file")) { fixture.file.bytes=UMI_MODEL_MAX_BYTES+1U;wanted=UMI_STATUS_PARSE_ERROR; }
    else if(MODE("read-error")) wanted=read_status=UMI_STATUS_IO_ERROR;
    else if(MODE("parse-error")) wanted=read_status=UMI_STATUS_PARSE_ERROR;
    else if(MODE("capacity")) wanted=read_status=UMI_STATUS_CAPACITY_EXCEEDED;
    else if(MODE("late-cancel")) { cancel_after_read=true;wanted=UMI_STATUS_CANCELLED; }
    else if(MODE("pre-cancel")) { umi_cancellation_token_request(token);wanted=UMI_STATUS_CANCELLED; }
    else if(MODE("null-path")) { path=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("null-output")) { destination=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("null-digest")) { reviewed=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("short-digest")) { expected[63]='\0';wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("uppercase-digest")) { expected[0]='D';wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else assert(MODE("valid"));
#else
    assert(MODE("unavailable") && !UmiTestModelRunInspectionAvailable());
    wanted=UMI_STATUS_NOT_IMPLEMENTED;
#endif
    memcpy(&before,&out,sizeof(before));
    assert(UmiTestModelInspectForRun(path,reviewed,token,destination)==wanted);
    if(wanted==UMI_STATUS_OK) {
        assert(out.bytes==128U && out.tensors==1U && out.metadata_entries==1U && out.gguf_version==3U);
        assert(strcmp(out.sha256,expected)==0);
    } else assert(memcmp(&before,&out,sizeof(out))==0);
#ifdef UMICOM_TEST_RUN_INSPECTION_ENABLED
    assert(reads==((wanted==UMI_STATUS_INVALID_ARGUMENT || MODE("pre-cancel"))?0U:1U));
#endif
    umi_cancellation_token_destroy(token);
    return 0;
}
