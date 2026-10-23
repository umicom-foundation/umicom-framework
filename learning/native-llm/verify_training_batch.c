/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-llm/verify_training_batch.c
 *
 * PURPOSE:
 *   Read-only native C23 release gate for multiple signed training snapshots.
 *   Accept explicit checkpoint paths, an independently reviewed publisher
 *   policy and an explicit UTC Unix evaluation second. No shell execution,
 *   network services, scripting runtime or model decoding is required.
 *
 * SECURITY:
 *   Failure produces no success JSON on stdout. JSON is unsigned evidence
 *   of a local verification observation, not host or signing-time attestation.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_trust_batch.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

static int ReadUnixSeconds(const char *text, uint64_t *outSeconds)
{
    uint64_t number = 0U;
    if (text == NULL || outSeconds == NULL || text[0] == '\0') return 0;
    if (text[0] == '0' && text[1] != '\0') return 0;
    for (const char *cursor = text; *cursor != '\0'; ++cursor) {
        const uint64_t digit = (uint64_t)(unsigned char)*cursor - (uint64_t)'0';
        if (*cursor < '0' || *cursor > '9' ||
            number > ((uint64_t)INT64_MAX - digit) / UINT64_C(10))
            return 0;
        number = number * UINT64_C(10) + digit;
    }
    *outSeconds = number;
    return 1;
}

int main(int argc, char **argv)
{
    uint64_t seconds;
    char json[UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY];
    const char *checkpointPaths[UMI_AI_TRAINING_TRUST_BATCH_MAX_CHECKPOINTS];
    size_t written = 0U;
    UmiStatus status;
    if (argc < 4 || argc > (int)UMI_AI_TRAINING_TRUST_BATCH_MAX_CHECKPOINTS + 3 ||
        !ReadUnixSeconds(argc >= 3 ? argv[2] : NULL, &seconds)) {
        fputs("usage: umicom-native-llm-trust-batch "
              "<approved-policy.trust> <UTC-unix-seconds> "
              "<checkpoint1.umi> [checkpoint2.umi ... checkpoint8.umi]\n"
              "All checkpoints must be signed and verified independently.\n",
              stderr);
        return 2;
    }
    for (int index = 3; index < argc; ++index)
        checkpointPaths[(size_t)(index - 3)] = argv[index];
    status = UmiAiTrainingTrustBatchVerifyJson(
        checkpointPaths, (size_t)(argc - 3),
        argv[1], seconds, json, sizeof(json), &written);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "training release batch rejected (status=%d)\n", (int)status);
        return 1;
    }
    /* There is no JSON output until ALL source files have passed verification.
     * As with any CLI, downstream consumers must also require exit code zero
     * and preserve the actual stdout bytes as the observation they received. */
    if (fwrite(json, 1U, written, stdout) != written ||
        fputc('\n', stdout) == EOF || fflush(stdout) != 0) {
        fputs("could not write complete verification batch\n", stderr);
        return 1;
    }
    return 0;
}
