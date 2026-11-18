/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/jsonl_review.c
 *
 * PURPOSE:
 *   Stage a reviewed membership assignment until both shared dataset checks complete.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_review.h"
#include "umicom/security/secrets.h"
#include <string.h>

UmiStatus UmiAiTrainingJsonlReview(const void *bytes,size_t length,
    const UmiAiTrainingJsonlReviewOptions *options,const UmiCancellationToken *cancel,
    unsigned char *outEvaluation,size_t capacity,UmiAiTrainingJsonlReviewSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport)
{
    if(outReport==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *outReport=(UmiAiTrainingJsonlReport){0};
    if(bytes==NULL || length==0U || options==NULL || outEvaluation==NULL || outSummary==NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if(options->evaluationGroupCount==0U || options->evaluationGroupCount>=UMI_AI_TRAINING_JSONL_ROWS ||
        options->maxStagingBytes==0U)return UMI_STATUS_INVALID_ARGUMENT;
    if(length>UMI_AI_TRAINING_JSONL_BYTES)return UMI_STATUS_CAPACITY_EXCEEDED;
    if(umi_cancellation_token_is_requested(cancel))return UMI_STATUS_CANCELLED;
    unsigned char candidate[UMI_AI_TRAINING_JSONL_ROWS]={0};
    UmiAiTrainingJsonlReviewSummary summary={0};
    /* Partition and overlap deliberately share the caller's immutable snapshot.
     * The first operation must not expose membership if the later audit fails. */
    UmiStatus status=UmiAiTrainingJsonlPartition(bytes,length,options->evaluationGroupCount,
        options->seed,cancel,candidate,sizeof(candidate),&summary.partition,outReport);
    if(status!=UMI_STATUS_OK)goto finished;
    if(capacity<summary.partition.rows){status=UMI_STATUS_CAPACITY_EXCEEDED;goto finished;}
    status=UmiAiTrainingJsonlAuditOverlap(bytes,length,candidate,summary.partition.rows,
        options->maxStagingBytes,cancel,&summary.overlap,outReport);
    if(status!=UMI_STATUS_OK)goto finished;
    if(umi_cancellation_token_is_requested(cancel)){status=UMI_STATUS_CANCELLED;goto finished;}
    memcpy(outEvaluation,candidate,summary.partition.rows);
    *outSummary=summary;
finished:
    umi_secret_clear(candidate,sizeof(candidate));
    return status;
}
