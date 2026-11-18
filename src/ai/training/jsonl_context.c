/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/jsonl_context.c
 *
 * PURPOSE:
 *   Compose strict record parsing with canonical byte-pair geometry and transactional context statistics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/training/jsonl_context.h"
#include <stdint.h>
#include <string.h>

static UmiStatus InspectPair(void *opaque,const UmiAiTrainingPair *pair,size_t line)
{
    UmiAiTrainingJsonlContextSummary *summary=opaque;
    if(summary->records>=UMI_AI_TRAINING_JSONL_ROWS || line!=summary->records+1U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiAiTrainingBytePairShape shape;
    /* The parser has already bounded, decoded and NUL-terminated both fields.
     * Count decoded bytes, never raw JSON escape characters or guessed tokens. */
    UmiStatus status=UmiAiTrainingBytePairMeasure(strlen(pair->prompt),strlen(pair->completion),&shape);
    if(status!=UMI_STATUS_OK) return status;
    if(shape.validRows>SIZE_MAX-summary->requiredRows)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    summary->requiredRows+=shape.validRows;
    if(summary->records==0U || shape.validRows<summary->shortestRequiredRows)
        summary->shortestRequiredRows=shape.validRows;
    if(shape.validRows>summary->longestRequiredRows)
        summary->longestRequiredRows=shape.validRows;
    ++summary->records;
    if(shape.validRows>summary->contextLength) {
        ++summary->overlongRecords;
        if(summary->firstOverlongLine==0U) summary->firstOverlongLine=line;
        return UMI_STATUS_OK;
    }
    const size_t padding=summary->contextLength-shape.validRows;
    if(shape.validRows>SIZE_MAX-summary->fittingValidRows ||
        shape.selectedRows>SIZE_MAX-summary->fittingSelectedRows ||
        padding>SIZE_MAX-summary->fittingPaddingRows)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    ++summary->fittingRecords;
    summary->fittingValidRows+=shape.validRows;
    summary->fittingSelectedRows+=shape.selectedRows;
    summary->fittingPaddingRows+=padding;
    return UMI_STATUS_OK;
}
UmiStatus UmiAiTrainingJsonlInspectContext(const void *bytes,size_t length,
    size_t contextLength,const UmiCancellationToken *cancel,
    UmiAiTrainingJsonlContextSummary *outSummary,UmiAiTrainingJsonlReport *outReport)
{
    if(outReport==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport=(UmiAiTrainingJsonlReport){0};
    if(outSummary==NULL || contextLength==0U || contextLength>UMI_AI_DECODER_MAX_CONTEXT)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingJsonlContextSummary candidate={.contextLength=contextLength};
    UmiStatus status=UmiAiTrainingJsonlVisit(bytes,length,cancel,InspectPair,&candidate,outReport);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status=UMI_STATUS_CANCELLED;
    /* Only a complete scan publishes statistics. Parser progress remains
     * available separately so a later bad row cannot masquerade as a small corpus. */
    if(status==UMI_STATUS_OK) *outSummary=candidate;
    return status;
}
