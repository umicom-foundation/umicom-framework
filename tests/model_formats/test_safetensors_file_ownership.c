/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/model_formats/test_safetensors_file_ownership.c
 *
 * PURPOSE:
 *   Verify actual streamed header capture across chunk boundaries and late reader failures with retained output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "safetensors_fixture.h"
#include "umicom/model_formats/safetensors_file.h"
#include "umicom/platform/input_stream.h"
#include <stdbool.h>
static const unsigned char *stream_bytes;
static size_t stream_size,chunk_size=7U,allocations,live_allocations,visits;
static bool refuse_allocation,fail_mid,zero_chunk,cancel_after_read;
static UmiStatus finish_status=UMI_STATUS_OK;
static UmiCancellationToken *stream_cancel;
static void *capture_malloc(size_t size)
{
    ++allocations;if(refuse_allocation) return NULL;
    void *memory=malloc(size);if(memory!=NULL) ++live_allocations;return memory;
}
static void capture_free(void *memory)
{
    if(memory!=NULL) --live_allocations;
    free(memory);
}
static UmiStatus capture_visit(const char *path,uint64_t maximum,UmiInputChunkVisitor visitor,
    void *context,uint64_t *out_size)
{
    (void)path;++visits;*out_size=0U;
    if(stream_size>maximum) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(zero_chunk) {
        UmiStatus status=visitor(NULL,0U,context);if(status!=UMI_STATUS_OK) return status;
    }
    for(size_t offset=0U;offset<stream_size;) {
        if(fail_mid && offset>=8U) return UMI_STATUS_IO_ERROR;
        size_t amount=stream_size-offset;if(amount>chunk_size) amount=chunk_size;
        UmiStatus status=visitor(stream_bytes+offset,amount,context);
        if(status!=UMI_STATUS_OK) return status;
        offset+=amount;
    }
    if(cancel_after_read) umi_cancellation_token_request(stream_cancel);
    if(finish_status==UMI_STATUS_OK) *out_size=(uint64_t)stream_size;
    return finish_status;
}
UmiStatus UmiTestSafetensorsFileInspect(const char *path,const UmiSafetensorsFileIdentity *expected,
    const UmiCancellationToken *cancel,UmiSafetensorsFileInspection *out);
/* Replace only the I/O boundary and observe the real capture allocation. The
 * native checked-reader suite owns real handle, metadata and close behavior. */
#define UmiInputFileVisit capture_visit
#define UmiSafetensorsFileInspect UmiTestSafetensorsFileInspect
#define malloc capture_malloc
#define free capture_free
#include "../../src/model_formats/safetensors_file.c"
#undef free
#undef malloc
#undef UmiSafetensorsFileInspect
#undef UmiInputFileVisit
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr,"line %d: %s\n",__LINE__,#condition); return 1; } } while (0)
static int exercise(const char *mode)
{
    size_t size;unsigned char *bytes=tensor_fixture_prefix(tensor_fixture_json,24U,&size);CHECK(bytes!=NULL);
    stream_bytes=bytes;stream_size=size;
    CHECK(umi_cancellation_token_create(&stream_cancel)==UMI_STATUS_OK);
    UmiStatus wanted=UMI_STATUS_OK;
    if(strcmp(mode,"one-byte")==0) chunk_size=1U;
    else if(strcmp(mode,"split-length")==0) chunk_size=7U;
    else if(strcmp(mode,"split-header")==0) chunk_size=13U;
    else if(strcmp(mode,"whole")==0) chunk_size=size;
    else if(strcmp(mode,"zero-chunk")==0) zero_chunk=true;
    else if(strcmp(mode,"allocation")==0) { refuse_allocation=true;wanted=UMI_STATUS_OUT_OF_MEMORY; }
    else if(strcmp(mode,"partial-read")==0) { fail_mid=true;wanted=UMI_STATUS_IO_ERROR; }
    else if(strcmp(mode,"late-close")==0) { finish_status=UMI_STATUS_IO_ERROR;wanted=UMI_STATUS_IO_ERROR; }
    else if(strcmp(mode,"changed-file")==0) { finish_status=UMI_STATUS_BUSY;wanted=UMI_STATUS_BUSY; }
    else if(strcmp(mode,"late-cancel")==0) { cancel_after_read=true;wanted=UMI_STATUS_CANCELLED; }
    else if(strcmp(mode,"truncated-header")==0) { stream_size=12U;wanted=UMI_STATUS_PARSE_ERROR; }
    else return 2;
    UmiSafetensorsFileInspection info;memset(&info,0xa5,sizeof(info));
    unsigned char before[sizeof(info)];memcpy(before,&info,sizeof(info));
    CHECK(UmiTestSafetensorsFileInspect("synthetic",NULL,stream_cancel,&info)==wanted);
    CHECK(visits==1U && allocations==1U && live_allocations==0U);
    if(wanted!=UMI_STATUS_OK) CHECK(memcmp(&info,before,sizeof(info))==0);
    else {
        CHECK(info.file.bytes==125U && info.layout.data_bytes==24U && info.layout.elements==6U);
        char digest[65];UmiSha256Hex(info.file.sha256,digest);
        CHECK(strcmp(digest,"ae7f60d3e0de6f1dd84a3aebebc19d38af6720bc42e8e8344f8ca2dc0cfbee47")==0);
    }
    refuse_allocation=false;fail_mid=false;cancel_after_read=false;finish_status=UMI_STATUS_OK;stream_size=size;
    umi_cancellation_token_reset(stream_cancel);
    CHECK(UmiTestSafetensorsFileInspect("synthetic",NULL,stream_cancel,&info)==UMI_STATUS_OK);
    CHECK(live_allocations==0U && info.file.bytes==125U);
    CHECK(UmiTestSafetensorsFileInspect("synthetic",&info.file,stream_cancel,&info)==UMI_STATUS_OK);
    CHECK(live_allocations==0U);
    size_t before_visits=visits,before_allocations=allocations;
    CHECK(UmiTestSafetensorsFileInspect("synthetic",NULL,stream_cancel,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(visits==before_visits && allocations==before_allocations);
    free(bytes);umi_cancellation_token_destroy(stream_cancel);return 0;
}
int main(int argc,char **argv) { return argc==2?exercise(argv[1]):2; }
