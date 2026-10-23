/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM educational laboratory
 * File: learning/native-llm/verify_training_trust.c
 *
 * PURPOSE:
 *   Independently review a signed checkpoint under an operator-selected,
 *   offline, bounded Ed25519 public-key trust policy.
 *
 * SECURITY:
 *   No model decoding, training, script execution or network connection.
 *   A missing trusted policy is an error; a signature never creates trust.
 *   Default verification time is the local machine's Unix clock, which an
 *   operator must independently trust. An explicitly supplied instant is
 *   useful for repeatable audits but does not establish signing time.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_trust_policy.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static int ReadSeconds(const char *text, uint64_t *out)
{
    uint64_t result = 0U;
    if (text == NULL || out == NULL || text[0] == '\0') return 0;
    if (text[0] == '0' && text[1] != '\0') return 0;
    for (const char *cursor = text; *cursor != '\0'; ++cursor) {
        uint64_t digit;
        if (*cursor < '0' || *cursor > '9') return 0;
        digit = (uint64_t)(unsigned)(*cursor - '0');
        if (result > (UINT64_MAX - digit) / UINT64_C(10)) return 0;
        result = result * UINT64_C(10) + digit;
    }
    if (result > (uint64_t)INT64_MAX) return 0;
    *out = result;
    return 1;
}

int main(int argc, char **argv)
{
    UmiAiTrainingTrustDecision evidence;
    uint64_t seconds;
    UmiStatus status;
    /* Before approving a key, derive its canonical identity from the actual
     * raw Ed25519 key bytes. PEM-container hashes are not interchangeable. */
    if (argc == 3 && strcmp(argv[1], "--fingerprint") == 0) {
        char fingerprint[UMI_SHA256_HEX_CAPACITY];
        status = UmiAiTrainingAttestationPublicKeyFingerprint(
            argv[2], fingerprint);
        if (status != UMI_STATUS_OK) {
            fprintf(stderr, "public key inspection failed (status=%d)\n",
                    (int)status);
            return 1;
        }
        printf("Ed25519 public-key SHA-256: %s\n", fingerprint);
        puts("Verify this fingerprint independently before approving a policy.");
        return 0;
    }
    if (argc != 3 && argc != 4) {
        fputs("usage: umicom-native-llm-publication-trust "
              "<reviewed-checkpoint.umi> <independently-approved-trust-policy> "
              "[verification-unix-seconds]\n"
              "       umicom-native-llm-publication-trust --fingerprint "
              "<reviewed-public-key.pem>\n", stderr);
        return 2;
    }
    if (argc == 4) {
        if (!ReadSeconds(argv[3], &seconds)) {
            fputs("invalid UTC Unix seconds\n", stderr);
            return 2;
        }
    } else {
        time_t now = time(NULL);
        if (now == (time_t)-1 || now < (time_t)0) {
            fputs("current UTC clock is unavailable; pass explicit seconds\n", stderr);
            return 2;
        }
        seconds = (uint64_t)now;
    }
    status = UmiAiTrainingAttestationVerifyWithTrustPolicy(
        argv[1], argv[2], seconds, &evidence);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "signed training publication rejected (status=%d)\n",
                (int)status);
        return 1;
    }
    printf("Verified Ed25519 publisher key: %s\n",
           evidence.verifiedPublicKeySha256);
    printf("Policy evaluated at UTC Unix second: %" PRIu64 "\n",
           evidence.evaluatedAtUnixSeconds);
    printf("Checkpoint bytes: %" PRIu64 "\n", evidence.publication.checkpoint.byteCount);
    printf("Checkpoint SHA-256: %s\n", evidence.publication.checkpoint.sha256);
    puts("Publisher key and four published files match the reviewed policy.");
    puts("This does not prove model correctness, source authorisation or signing time.");
    return 0;
}
