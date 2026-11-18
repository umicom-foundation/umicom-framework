/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/jsonl_file.c
 *
 * PURPOSE:
 *   Compose regular-file admission with transactional dataset statistics and scoped input storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_file.h"
#include "umicom/platform/input_file.h"
#include "umicom/security/secrets.h"

/* The original summary-only reader is retained for review. The shared helper
 * below preserves its admission, cancellation and clearing contract while
 * allowing context inspection to reuse exactly the same file lifetime. */
#if 0
UmiStatus UmiAiTrainingJsonlInspectFile(const char *absolutePath,
    const UmiCancellationToken *cancel, UmiAiTrainingJsonlSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport)
{
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport = (UmiAiTrainingJsonlReport){0};
    if (absolutePath == NULL || outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    /* Shared platform admission owns file/handle checks. Reuse the same
     * summary operation as in-memory clients; applications do not parse JSON. */
    UmiStatus status = UmiInputFileRead(absolutePath,
        UMI_AI_TRAINING_JSONL_BYTES, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingJsonlSummarize(bytes, size, cancel, outSummary, outReport);
    if (bytes != NULL) {
        umi_secret_clear(bytes, size + 1U);
        UmiInputFileFree(bytes);
    }
    return status;
}
#endif

typedef UmiStatus (*InspectBytes)(const void *,size_t,size_t,
    const UmiCancellationToken *,void *,UmiAiTrainingJsonlReport *);

static UmiStatus SummaryBytes(const void *bytes,size_t length,size_t contextLength,
    const UmiCancellationToken *cancel,void *summary,UmiAiTrainingJsonlReport *report)
{
    (void)contextLength;
    return UmiAiTrainingJsonlSummarize(bytes,length,cancel,summary,report);
}
static UmiStatus ContextBytes(const void *bytes,size_t length,size_t contextLength,
    const UmiCancellationToken *cancel,void *summary,UmiAiTrainingJsonlReport *report)
{
    return UmiAiTrainingJsonlInspectContext(bytes,length,contextLength,cancel,summary,report);
}
static UmiStatus InspectSelectedFile(const char *absolutePath,size_t contextLength,
    const UmiCancellationToken *cancel,void *outSummary,
    UmiAiTrainingJsonlReport *outReport,InspectBytes inspect)
{
    if(outReport==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport=(UmiAiTrainingJsonlReport){0};
    if(absolutePath==NULL || outSummary==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    unsigned char *bytes=NULL;
    size_t size=0U;
    /* One file owner serves both aggregate readers. The operation receives a
     * synchronous borrow and may not retain bytes after this helper clears them. */
    UmiStatus status=UmiInputFileRead(absolutePath,UMI_AI_TRAINING_JSONL_BYTES,&bytes,&size);
    if(status==UMI_STATUS_OK)
        status=inspect(bytes,size,contextLength,cancel,outSummary,outReport);
    if(bytes!=NULL) {
        umi_secret_clear(bytes,size+1U);
        UmiInputFileFree(bytes);
    }
    return status;
}
UmiStatus UmiAiTrainingJsonlInspectFile(const char *absolutePath,
    const UmiCancellationToken *cancel,UmiAiTrainingJsonlSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport)
{
    return InspectSelectedFile(absolutePath,0U,cancel,outSummary,outReport,SummaryBytes);
}
UmiStatus UmiAiTrainingJsonlInspectContextFile(const char *absolutePath,
    size_t contextLength,const UmiCancellationToken *cancel,
    UmiAiTrainingJsonlContextSummary *outSummary,UmiAiTrainingJsonlReport *outReport)
{
    if(outReport==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport=(UmiAiTrainingJsonlReport){0};
    if(contextLength==0U || contextLength>UMI_AI_DECODER_MAX_CONTEXT)
        return UMI_STATUS_INVALID_ARGUMENT;
    return InspectSelectedFile(absolutePath,contextLength,cancel,outSummary,outReport,ContextBytes);
}


/* The shared file helper lends acquired bytes to one operation. Carrying this
 * request as its opaque output context reuses admission and clearing unchanged. */
typedef struct ReviewFileRequest {
    const UmiAiTrainingJsonlReviewOptions *options;
    unsigned char *membership;
    size_t capacity;
    UmiAiTrainingJsonlReviewSummary *summary;
} ReviewFileRequest;
static UmiStatus ReviewBytes(const void *bytes,size_t length,size_t unused,
    const UmiCancellationToken *cancel,void *opaque,UmiAiTrainingJsonlReport *report)
{
    (void)unused;
    ReviewFileRequest *request=opaque;
    return UmiAiTrainingJsonlReview(bytes,length,request->options,cancel,
        request->membership,request->capacity,request->summary,report);
}
UmiStatus UmiAiTrainingJsonlReviewFile(const char *absolutePath,
    const UmiAiTrainingJsonlReviewOptions *options,const UmiCancellationToken *cancel,
    unsigned char *outEvaluation,size_t capacity,UmiAiTrainingJsonlReviewSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport)
{
    if(outReport==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *outReport=(UmiAiTrainingJsonlReport){0};
    /* Reject malformed options before reading even a missing or oversized file. */
    if(options==NULL || outEvaluation==NULL || outSummary==NULL ||
        options->evaluationGroupCount==0U || options->evaluationGroupCount>=UMI_AI_TRAINING_JSONL_ROWS ||
        options->maxStagingBytes==0U)return UMI_STATUS_INVALID_ARGUMENT;
    ReviewFileRequest request={options,outEvaluation,capacity,outSummary};
    return InspectSelectedFile(absolutePath,0U,cancel,&request,outReport,ReviewBytes);
}
