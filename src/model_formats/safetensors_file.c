/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/model_formats/safetensors_file.c
 *
 * PURPOSE:
 *   Combine checked streaming, incremental hashing and bounded model layout admission before publishing observed file identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/model_formats/safetensors_file.h"
#include "umicom/platform/input_stream.h"
#include "header_internal.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct TensorFileCapture {
    UmiSha256 hash;
    const UmiCancellationToken *cancel;
    unsigned char length_bytes[8];
    size_t length_used;
    unsigned char *header;
    size_t header_size,header_used;
} TensorFileCapture;
static UmiStatus tensor_file_chunk(const unsigned char *bytes,size_t size,void *context)
{
    TensorFileCapture *capture=context;
    if(umi_cancellation_token_is_requested(capture->cancel)) return UMI_STATUS_CANCELLED;
    size_t consumed=0U;
    if(capture->length_used<8U) {
        size_t take=8U-capture->length_used;
        if(take>size) take=size;
        if(take!=0U) memcpy(capture->length_bytes+capture->length_used,bytes,take);
        capture->length_used+=take;consumed=take;
    }
    if(capture->length_used==8U && capture->header==NULL) {
        uint64_t length;
        UmiStatus status=SafetensorsReadHeaderLength(capture->length_bytes,&length);
        if(status!=UMI_STATUS_OK) return status;
        capture->header_size=8U+(size_t)length;
        capture->header=malloc(capture->header_size);
        if(capture->header==NULL) return UMI_STATUS_OUT_OF_MEMORY;
        memcpy(capture->header,capture->length_bytes,8U);capture->header_used=8U;
    }
    if(capture->header!=NULL && capture->header_used<capture->header_size) {
        size_t take=capture->header_size-capture->header_used;
        if(take>size-consumed) take=size-consumed;
        if(take!=0U) memcpy(capture->header+capture->header_used,bytes+consumed,take);
        capture->header_used+=take;
    }
    return UmiSha256Update(&capture->hash,bytes,size);
}
UmiStatus UmiSafetensorsFileInspect(const char *path,const UmiSafetensorsFileIdentity *expected,
    const UmiCancellationToken *cancel,UmiSafetensorsFileInspection *out_inspection)
{
    if(path==NULL || out_inspection==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Snapshot a borrowed previous identity before doing any work. All output
     * remains staged, including after late reader/close/cancellation failures. */
    UmiSafetensorsFileIdentity reviewed={0};
    if(expected!=NULL) {
        reviewed=*expected;
        if(reviewed.bytes<10U || reviewed.bytes>UMI_SAFETENSORS_MAX_FILE_BYTES)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    if(umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    TensorFileCapture capture={0};capture.cancel=cancel;
    UmiSha256Init(&capture.hash);
    UmiSafetensorsFileInspection candidate={0};
    uint64_t maximum=expected!=NULL?reviewed.bytes:UMI_SAFETENSORS_MAX_FILE_BYTES;
    UmiStatus status=UmiInputFileVisit(path,maximum,tensor_file_chunk,&capture,&candidate.file.bytes);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel)) status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK && (capture.length_used!=8U || capture.header==NULL ||
        capture.header_used!=capture.header_size)) status=UMI_STATUS_PARSE_ERROR;
    if(status==UMI_STATUS_OK) status=UmiSha256Final(&capture.hash,candidate.file.sha256);
    if(status==UMI_STATUS_OK && expected!=NULL &&
        (candidate.file.bytes!=reviewed.bytes || memcmp(candidate.file.sha256,reviewed.sha256,UMI_SHA256_BYTES)!=0))
        status=UMI_STATUS_INVALID_STATE;
    if(status==UMI_STATUS_OK)
        status=UmiSafetensorsHeaderInspect(capture.header,capture.header_used,
            candidate.file.bytes,cancel,&candidate.layout);
    free(capture.header);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel)) status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK) *out_inspection=candidate;
    return status;
}
