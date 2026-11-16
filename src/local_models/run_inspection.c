/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/local_models/run_inspection.c
 *
 * PURPOSE:
 *   Apply shared bounded metadata and digest admission before a local text-model process can be submitted.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "internal.h"
#include "umicom/base/sha256.h"
#include <string.h>
#ifdef UMICOM_MODELS_HAS_TENSOR_INSPECTION
#include "umicom/model_formats/gguf.h"
#endif

bool UmiModelRunInspectionAvailable(void)
{
#ifdef UMICOM_MODELS_HAS_TENSOR_INSPECTION
    return true;
#else
    return false;
#endif
}
UmiStatus UmiModelInspectForRun(const char *path,const char *expectedSha256,
    const UmiCancellationToken *cancellation,UmiModelInspection *outInspection)
{
    if(path==NULL || outInspection==NULL || !UmiModelDigestValid(expectedSha256,64U))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Capture a digest borrowed from the previous output before publication.
     * A failed observation leaves that previous complete result usable. */
    char expected[65];memcpy(expected,expectedSha256,sizeof(expected));
    if(umi_cancellation_token_is_requested(cancellation)) return UMI_STATUS_CANCELLED;
#ifdef UMICOM_MODELS_HAS_TENSOR_INSPECTION
    UmiGgufFileInspection observed;
    UmiStatus status=UmiGgufFileInspect(path,NULL,cancellation,&observed);
    if(status!=UMI_STATUS_OK) return status;
    UmiModelInspection candidate={0};
    candidate.bytes=observed.file.bytes;
    candidate.gguf_version=observed.metadata.version;
    candidate.tensors=observed.metadata.tensors;
    candidate.metadata_entries=observed.metadata.entries;
    UmiSha256Hex(observed.file.sha256,candidate.sha256);
    if(strcmp(expected,candidate.sha256)!=0) return UMI_STATUS_INVALID_STATE;
    /* Keep the original coarse count bounds in addition to role admission.
     * They reject impossible counts but do not validate tensor descriptors.
     * One checked read supplies both metadata and identity; never hash the
     * same potentially huge model a second time just to inspect its role. */
    if(candidate.bytes<24U || candidate.bytes>UMI_MODEL_MAX_BYTES ||
       candidate.tensors==0U || candidate.tensors>(candidate.bytes-24U)/16U ||
       candidate.metadata_entries>(candidate.bytes-24U)/12U)
        return UMI_STATUS_PARSE_ERROR;
    if(observed.metadata.role!=UMI_GGUF_ROLE_MODEL_CANDIDATE)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if(umi_cancellation_token_is_requested(cancellation)) return UMI_STATUS_CANCELLED;
    *outInspection=candidate;
    return UMI_STATUS_OK;
#else
    /* A selectively installed SDK may intentionally omit format inspection.
     * Preserve discovery/download APIs, but do not silently weaken launch
     * admission to the legacy header-only check in that composition. */
    (void)expected;
    return UMI_STATUS_NOT_IMPLEMENTED;
#endif
}
