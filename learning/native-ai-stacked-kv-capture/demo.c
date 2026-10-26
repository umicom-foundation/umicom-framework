/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-stacked-kv-capture/demo.c
 * PURPOSE: Explain actual canonical computation versus stored K/V rows using
 *          a fully synthetic, bounded multi-layer Transformer.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_capture.h"
#include <inttypes.h>
#include <stdio.h>

int main(void)
{
    const UmiAiTrainingStackedDecoderConfig config = {
        260U, 4U, 4U, 8U, 2U, 2U, UINT64_C(20261026)
    };
    const size_t tokens[] = {69U, 70U};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedKvCapture *capture = NULL;
    UmiAiTrainingStackedKvCaptureInfo info = {0};
    float logits[260U] = {0};
    UmiStatus status = UmiAiTrainingStackedDecoderCreate(&config, &model);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedKvCaptureCreate(model, &capture);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedKvCapturePrefill(
            capture, tokens, 2U, logits, 260U);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedKvCaptureAppend(
            capture, 71U, logits, 260U);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedKvCaptureAppend(
            capture, 72U, logits, 260U);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedKvCaptureAppend(
            capture, 73U, logits, 260U);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedKvCaptureInspect(capture, &info);
    if (status == UMI_STATUS_OK) {
        printf("Actual full-context token-layer rows: %" PRIu64 "\n",
                info.actuallyEvaluatedTokenLayerRows);
        printf("Stored K/V token-layer rows: %" PRIu64 "\n",
                info.stored.acceptedTokenLayerRows);
        printf("Full learned-position rebuilds: %" PRIu64 "\n",
                info.stored.fullWindowRebuilds);
        puts("Storage row savings are not inference speed improvements.");
    }
    UmiAiTrainingStackedKvCaptureDestroy(capture);
    UmiAiTrainingStackedDecoderDestroy(model);
    return status == UMI_STATUS_OK ? 0 : 1;
}
