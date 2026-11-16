/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/model_formats/gguf_file.c
 *
 * PURPOSE:
 *   Bind incremental GGUF role metadata to a complete checked file digest before publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/model_formats/gguf.h"
#include "umicom/platform/input_stream.h"
#include "gguf_internal.h"
#include <string.h>
typedef struct GgufFileCapture {
    GgufScan scan;
    UmiSha256 hash;
} GgufFileCapture;
static UmiStatus gguf_chunk(const unsigned char *bytes,size_t size,void *context)
{
    GgufFileCapture *capture=context;
    if(umi_cancellation_token_is_requested(capture->scan.cancel)) return UMI_STATUS_CANCELLED;
    UmiStatus status=GgufScanFeed(&capture->scan,bytes,size);
    if(status==UMI_STATUS_OK) status=UmiSha256Update(&capture->hash,bytes,size);
    return status;
}
UmiStatus UmiGgufFileInspect(const char *path,const UmiGgufFileIdentity *expected,
    const UmiCancellationToken *cancel,UmiGgufFileInspection *out_inspection)
{
    if(path==NULL || out_inspection==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiGgufFileIdentity reviewed={0};
    if(expected!=NULL) {
        reviewed=*expected;
        if(reviewed.bytes<24U || reviewed.bytes>UMI_GGUF_MAX_FILE_BYTES) return UMI_STATUS_INVALID_ARGUMENT;
    }
    if(umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    GgufFileCapture capture;
    GgufScanInit(&capture.scan,cancel);UmiSha256Init(&capture.hash);
    UmiGgufFileInspection candidate={0};
    UmiStatus status=UmiInputFileVisit(path,expected==NULL?UMI_GGUF_MAX_FILE_BYTES:reviewed.bytes,
        gguf_chunk,&capture,&candidate.file.bytes);
    if(status==UMI_STATUS_OK) status=GgufScanFinish(&capture.scan,&candidate.metadata);
    if(status==UMI_STATUS_OK) status=UmiSha256Final(&capture.hash,candidate.file.sha256);
    if(status==UMI_STATUS_OK && expected!=NULL &&
       (reviewed.bytes!=candidate.file.bytes || memcmp(reviewed.sha256,candidate.file.sha256,UMI_SHA256_BYTES)!=0))
        status=UMI_STATUS_INVALID_STATE;
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel)) status=UMI_STATUS_CANCELLED;
    /* Commit role and full-file identity together after the checked reader has
     * closed its handle. A correct prefix cannot hide a late I/O failure. */
    if(status==UMI_STATUS_OK) *out_inspection=candidate;
    return status;
}
