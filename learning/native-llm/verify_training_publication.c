/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-llm/verify_training_publication.c
 *
 * PURPOSE:
 *   Read-only C23 CLI for offline integrity checking of a published native
 *   AdamW checkpoint plus its three reviewed policy companions.
 *
 * IMPORTANT:
 *   This command checks recorded byte identities. It does NOT authenticate
 *   the publisher, execute a model, attest the host or validate training
 *   quality. Independently perform the existing strict policy/source checks.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_publication.h"

#include <inttypes.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    UmiAiTrainingPublicationEvidence evidence;
    UmiStatus status;
    if (argc != 2) {
        fputs("usage: umicom-native-llm-publication-verify "
              "<reviewed-combined-checkpoint>\n", stderr);
        return 2;
    }
    /* Obtain the displayed file identities from the SAME read pass that
     * passed verification; do not start a second unsynchronised inspection. */
    status = UmiAiTrainingPublicationVerifyAndInspectFile(argv[1], &evidence);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "training publication missing/incomplete/mismatched "
                "(status=%d)\n", (int)status);
        return 1;
    }
    printf("Local checkpoint publication: byte identities verified\n"
           "  checkpoint (%" PRIu64 " bytes): %s\n"
           "  clip policy (%" PRIu64 " bytes): %s\n"
           "  training lock (%" PRIu64 " bytes): %s\n"
           "  source lock (%" PRIu64 " bytes): %s\n",
           evidence.checkpoint.byteCount, evidence.checkpoint.sha256,
           evidence.clipPolicy.byteCount, evidence.clipPolicy.sha256,
           evidence.trainingLock.byteCount, evidence.trainingLock.sha256,
           evidence.sourceLock.byteCount, evidence.sourceLock.sha256);
    puts("Unsigned digest evidence is not a signature or host attestation.");
    return 0;
}
