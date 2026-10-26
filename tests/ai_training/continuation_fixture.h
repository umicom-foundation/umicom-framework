/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/continuation_fixture.h
 * PURPOSE: Keep native continuation regression fixtures strictly synthetic.
 *          Actual source review remains a production owner responsibility.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_CONTINUATION_FIXTURE_H
#define UMICOM_AI_TRAINING_CONTINUATION_FIXTURE_H

#include "supervised_fixture.h"
#include "umicom/ai/training/supervised_continuation.h"

static inline UmiStatus ContinuationCreateFixture(
    UmiTestSupervisedFixture *fixture,
    UmiAiTrainingDecoder **outDecoder,
    UmiAiTrainingSupervisedDataset **outDataset,
    UmiAiTrainingDecoderProjectionAdapter **outAdapter)
{
    const UmiAiTrainingDecoderConfig config = {
        260U, 8U, 8U, 16U, 2U, UINT64_C(77011)
    };
    UmiStatus status;
    if (fixture == NULL || outDecoder == NULL || outDataset == NULL ||
        outAdapter == NULL || *outDecoder != NULL || *outDataset != NULL ||
        *outAdapter != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiTestSupervisedFixturePrepare(fixture);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingSupervisedDatasetCreate(fixture->records,
        fixture->lengths, fixture->evidence, 4U, 8U, outDataset);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingDecoderCreate(&config, outDecoder);
    if (status != UMI_STATUS_OK) return status;
    return UmiAiTrainingDecoderProjectionAdapterCreate(
        *outDecoder, UMI_AI_DECODER_ADAPT_QUERY |
                     UMI_AI_DECODER_ADAPT_VALUE, 2U, UINT64_C(29), outAdapter);
}

static inline void ContinuationDestroyFixture(
    UmiAiTrainingDecoder *decoder,
    UmiAiTrainingSupervisedDataset *dataset,
    UmiAiTrainingDecoderProjectionAdapter *adapter)
{
    UmiAiTrainingDecoderProjectionAdapterDestroy(adapter);
    UmiAiTrainingDecoderDestroy(decoder);
    UmiAiTrainingSupervisedDatasetDestroy(dataset);
}
#endif
