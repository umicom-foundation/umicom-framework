/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM educational laboratory
 * File: learning/native-llm/verify_signed_release_approval.c
 *
 * PURPOSE:
 *   Sign one locally reviewed release approval, or verify that a separately
 *   trusted approver signed it before checking a publisher-approved release.
 *   All algorithms, path checks and policy decisions are Framework-owned.
 *
 * SECURITY:
 *   The signer supplies a private PEM deliberately; it is never generated or
 *   saved by this executable. A verifier must obtain the approver public key
 *   and publisher trust policy independently of release artefacts. Nothing
 *   executes source files or makes network requests. Process exit codes are
 *   necessary to distinguish a complete JSON receipt from a broken pipe.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_approval_attestation.h"

#include <stdio.h>
#include <string.h>

static int Usage(void)
{
    fputs("usage:\n"
          "  umicom-native-llm-signed-release sign <reviewed-approval-file> "
          "<private-Ed25519-PEM>\n"
          "  umicom-native-llm-signed-release verify <reviewed-approval-file> "
          "<trusted-approver-public-PEM> <publisher-trust-policy> "
          "<checkpoint.umi> [checkpoint.umi ...]\n", stderr);
    return 2;
}

int main(int argc, char **argv)
{
    UmiStatus status;
    if (argc == 4 && strcmp(argv[1], "sign") == 0) {
        status = UmiAiTrainingApprovalAttestationSignNewFile(
            argv[2], argv[3]);
        if (status != UMI_STATUS_OK) {
            fprintf(stderr, "approval signature not published (status=%d)\n",
                    (int)status);
            return 1;
        }
        puts("Signed approval companion created; protect the independent key.");
        return 0;
    }
    if (argc >= 6 && strcmp(argv[1], "verify") == 0 &&
        argc <= 5 + (int)UMI_AI_TRAINING_TRUST_BATCH_MAX_CHECKPOINTS) {
        char receipt[UMI_AI_TRAINING_SIGNED_APPROVAL_JSON_CAPACITY];
        size_t written = 0U;
        status = UmiAiTrainingSignedReleaseApprovalVerifyJson(
            (const char *const *)&argv[5], (size_t)(argc - 5),
            argv[4], argv[2], argv[3], receipt, sizeof(receipt), &written);
        if (status != UMI_STATUS_OK) {
            fprintf(stderr, "signed release approval rejected (status=%d)\n",
                    (int)status);
            return 1;
        }
        if (fwrite(receipt, 1U, written, stdout) != written ||
            fputc('\n', stdout) == EOF || fflush(stdout) != 0) {
            fputs("could not publish complete verification receipt\n", stderr);
            return 1;
        }
        return 0;
    }
    return Usage();
}
