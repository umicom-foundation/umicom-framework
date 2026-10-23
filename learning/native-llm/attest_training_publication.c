/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM educational laboratory
 * File: learning/native-llm/attest_training_publication.c
 *
 * PURPOSE:
 *   A thin native C23 interface for optional, offline Ed25519 attestation.
 *   All cryptography, publication integrity and file persistence belong to
 *   the Umicom Framework. This executable is never a signing daemon.
 *
 * OPERATIONAL SECURITY:
 *   Use only reviewed evidence, an approved clean signing host and private
 *   keys stored outside Git. Verification requires a separately trusted
 *   PUBLIC key. Signatures are neither antivirus clearance nor correctness
 *   certifications. No secrets or source files are printed to the console.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_attestation.h"

#include <stdio.h>
#include <string.h>

static void Usage(void)
{
    fputs("usage:\n"
          "  umicom-native-llm-publication-attest sign <published-checkpoint> <private-ed25519-key.pem>\n"
          "  umicom-native-llm-publication-attest verify <published-checkpoint> <trusted-ed25519-public-key.pem>\n",
          stderr);
}

int main(int argc, char **argv)
{
    UmiStatus status;
    if (argc != 4 || argv[1] == NULL || argv[2] == NULL || argv[3] == NULL) {
        Usage();
        return 2;
    }
    if (strcmp(argv[1], "sign") == 0) {
        status = UmiAiTrainingAttestationSignNewFile(argv[2], argv[3]);
        if (status == UMI_STATUS_OK) {
            puts("Ed25519 attestation published to a new, no-clobber sidecar.");
            puts("The signature identifies exact publication bytes, not model correctness.");
            return 0;
        }
        fprintf(stderr, "attestation refused (status=%d)\n", (int)status);
        return 1;
    }
    if (strcmp(argv[1], "verify") == 0) {
        status = UmiAiTrainingAttestationVerifyFile(argv[2], argv[3]);
        if (status == UMI_STATUS_OK) {
            puts("Ed25519 signature matches the separately trusted key, and all four published file hashes match.");
            puts("Source authorisation, host security and training quality remain independent checks.");
            return 0;
        }
        fprintf(stderr, "attestation verification rejected (status=%d)\n",
                (int)status);
        return 1;
    }
    Usage();
    return 2;
}
