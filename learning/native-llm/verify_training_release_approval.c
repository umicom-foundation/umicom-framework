/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-llm/verify_training_release_approval.c
 *
 * PURPOSE:
 *   Read a reviewer-controlled release approval, verify each signed native
 *   training publication under the operator's offline publisher trust policy,
 *   and emit the approved ordered JSON only after complete verification.
 *
 * SECURITY:
 *   An approval file must be independently approved and stored separately
 *   from untrusted release artefacts. This program neither creates approval
 *   records nor chooses trusted publishers. A matching unsigned SHA-256 does
 *   not attest to a trusted clock or host or the model's capabilities.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_release_approval.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    UmiAiTrainingReleaseApproval approval;
    char json[UMI_AI_TRAINING_RELEASE_APPROVAL_JSON_CAPACITY];
    size_t written = 0U;
    UmiStatus status;

    if (argc < 4 || argc > 3 +
        (int)UMI_AI_TRAINING_TRUST_BATCH_MAX_CHECKPOINTS) {
        fputs("usage: umicom-native-llm-release-approve "
              "<independently-approved-release-file> "
              "<trusted-publisher-policy> <checkpoint.umi> "
              "[checkpoint.umi ...]\n", stderr);
        return 2;
    }

    status = UmiAiTrainingReleaseApprovalLoadFile(argv[1], &approval);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "release approval file rejected (status=%d)\n",
                (int)status);
        return 1;
    }
    if (approval.publicationCount != (unsigned)(argc - 3)) {
        fputs("checkpoint count disagrees with independently approved release\n",
              stderr);
        return 1;
    }
    status = UmiAiTrainingReleaseApprovalVerifyJson(
        (const char *const *)&argv[3], (size_t)(argc - 3),
        argv[2], &approval, json, sizeof(json), &written);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "release not approved (status=%d)\n", (int)status);
        return 1;
    }

    /* This CLI never prints a partial success on verification failure.
     * A failure in stdout delivery can still leave partial bytes with a
     * non-zero exit status; consumers MUST check the process exit status. */
    if (fwrite(json, 1U, written, stdout) != written ||
        fputc('\n', stdout) == EOF || fflush(stdout) != 0) {
        fputs("could not write full release approval receipt\n", stderr);
        return 1;
    }
    return 0;
}
