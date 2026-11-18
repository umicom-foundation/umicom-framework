/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/byte_pair_sequence.c
 *
 * PURPOSE:
 *   Compose the canonical byte tokenizer into a failure-atomic padded decoder example without allocation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/byte_pair_sequence.h"

UmiStatus UmiAiTrainingBytePairPrepare(const uint8_t *prompt, size_t promptBytes,
    const uint8_t *completion, size_t completionBytes, size_t contextLength,
    UmiAiTrainingBytePairSequence *outSequence)
{
    if(outSequence==NULL || (promptBytes!=0U && prompt==NULL) ||
        completion==NULL || completionBytes==0U || contextLength==0U ||
        contextLength>UMI_AI_DECODER_MAX_CONTEXT)
        return UMI_STATUS_INVALID_ARGUMENT;
/* The shared shape service now owns overflow checks for both preparation and dataset inspection; retain the predecessor for contract review. The former implementation is retained for owner review. */
#if 0
    /* Reserve one prediction row for EOS before adding caller lengths. This
     * subtraction admits the total without allowing size_t addition to wrap. */
    if(promptBytes>contextLength-1U || completionBytes>contextLength-1U-promptBytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t valid=promptBytes+completionBytes+1U;
#endif
    UmiAiTrainingBytePairShape shape;
    UmiStatus shapeStatus=UmiAiTrainingBytePairMeasure(promptBytes,completionBytes,&shape);
    if(shapeStatus!=UMI_STATUS_OK) return shapeStatus;
    if(shape.validRows>contextLength) return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t valid=shape.validRows;
    uint32_t stream[UMI_AI_DECODER_MAX_CONTEXT+1U];
    size_t encoded=0U;
    stream[0]=UMI_AI_BYTE_BOS;
    UmiStatus status=UmiAiByteEncode(prompt,promptBytes,stream+1U,promptBytes,&encoded);
    if(status!=UMI_STATUS_OK) return status;
    if(encoded!=promptBytes) return UMI_STATUS_INVALID_STATE;
    status=UmiAiByteEncode(completion,completionBytes,stream+1U+promptBytes,completionBytes,&encoded);
    if(status!=UMI_STATUS_OK) return status;
    if(encoded!=completionBytes) return UMI_STATUS_INVALID_STATE;
    stream[valid]=UMI_AI_BYTE_EOS;

    UmiAiTrainingBytePairSequence candidate={0};
    candidate.context_length=contextLength;
    candidate.valid_rows=valid;
    /* Selected-target geometry comes from the same checked measurement. */
#if 0
    candidate.selected_rows=completionBytes+1U;
#endif
    candidate.selected_rows=shape.selectedRows;
    for(size_t i=0U;i<UMI_AI_DECODER_MAX_CONTEXT;++i) {
        candidate.inputs[i]=i<valid ? (size_t)stream[i] : UMI_AI_BYTE_PAD;
        candidate.targets[i]=i<valid ? (size_t)stream[i+1U] : UMI_AI_BYTE_PAD;
        candidate.attention[i]=(unsigned char)(i<valid ? 1U : 0U);
        candidate.selected[i]=(unsigned char)(i>=promptBytes && i<valid ? 1U : 0U);
    }
    /* Publish all correlated arrays together only after tokenizer composition
     * succeeds. The caller can pass them directly to the padded decoder API. */
    *outSequence=candidate;
    return UMI_STATUS_OK;
}
