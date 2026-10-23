/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Learning Laboratory
 * File: learning/native-llm/prefix-work/inspect_position_qualification.c
 * PURPOSE:
 *   Present real-decoder parity for growing-prefix K/V reuse and mandatory
 *   first-rollover rebuilding. No independently implemented model maths.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/kv_position_qualification.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261023)
    };
    const size_t input[] = {109U, 114U, 120U, 36U,
                            112U, 118U, 121U, 45U};
    const size_t nextTokenId = 37U;
    UmiAiTrainingDecoder *decoder = NULL;
    UmiAiTrainingKvPositionQualification report = {0};
    UmiStatus status;
    (void)argv;
    if (argc != 1) {
        fputs("usage: umicom-native-llm-position-qualification\n", stderr);
        return 2;
    }
    status = UmiAiTrainingDecoderCreate(&config, &decoder);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingKvPositionQualify(decoder,
            input, sizeof input / sizeof input[0], nextTokenId, &report);
    UmiAiTrainingDecoderDestroy(decoder);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "canonical/KV qualification rejected: %d\n", (int)status);
        return 1;
    }
    printf("Umicom Native LLM — positional cache qualification\n"
           "Decoder vocabulary: %zu\n"
           "Context tokens: %zu\n"
           "Canonical logits checked: %zu\n"
           "Growing-prefix computed rows: %" PRIu64 "\n"
           "Rows after FIRST context rollover: %" PRIu64 "\n"
           "Full-window rebuilds: %" PRIu64 "\n"
           "Cache heap reservation: %zu bytes\n"
           "Max difference before rollover: %.8f\n"
           "Max difference after rollover: %.8f\n",
           report.vocabularySize, report.contextTokens, report.comparedLogits,
           report.computedRowsBeforeRollover,
           report.computedRowsAfterRollover, report.fullWindowRebuilds,
           report.cacheHeapBytes,
           (double)report.maximumAbsoluteDifferenceBeforeRollover,
           (double)report.maximumAbsoluteDifferenceAfterRollover);
    if (report.computedRowsAfterRollover !=
            report.computedRowsBeforeRollover * UINT64_C(2) ||
        report.fullWindowRebuilds != UINT64_C(1)) {
        fputs("unexpected window-rebuild accounting\n", stderr);
        return 1;
    }
    puts("Both stages match the canonical decoder within 0.00004 per logit.");
    puts("No rolling-window speedup is asserted for learned absolute positions.");
    return 0;
}
