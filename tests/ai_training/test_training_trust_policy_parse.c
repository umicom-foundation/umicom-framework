/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_trust_policy_parse.c
 * PURPOSE: Qualify strict, failure-atomic public-key allowlist parsing.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_trust_policy.h"
#include <stdio.h>
#include <string.h>

#define KEY_A "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
#define KEY_B "abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

static int RejectWithoutModification(const char *text)
{
    UmiAiTrainingTrustPolicy output;
    UmiAiTrainingTrustPolicy preserved;
    memset(&output, 0xA5, sizeof(output));
    preserved = output;
    CHECK(UmiAiTrainingTrustPolicyParse(text, strlen(text), &output) !=
          UMI_STATUS_OK);
    CHECK(memcmp(&output, &preserved, sizeof(output)) == 0);
    return 0;
}

int main(void)
{
    UmiAiTrainingTrustPolicy output = {0};
    const char valid[] =
        "# independently controlled verification keyring\r\n"
        "UMICOM-AI-TRUST-1\r\n"
        "key=" KEY_A ",keys/active.pem,1700000000,2000000000,active\r\n"
        "key=" KEY_B ",keys/old.pem,0,1700000000,revoked\r\n";
    CHECK(UmiAiTrainingTrustPolicyParse(valid, strlen(valid), &output) ==
          UMI_STATUS_OK);
    CHECK(output.keyCount == 2U);
    CHECK(output.keys[0].validFromUnixSeconds == UINT64_C(1700000000));
    CHECK(output.keys[0].validUntilUnixSeconds == UINT64_C(2000000000));
    CHECK(output.keys[0].state == UMI_AI_TRAINING_TRUST_KEY_ACTIVE);
    CHECK(strcmp(output.keys[0].relativePemPath, "keys/active.pem") == 0);
    CHECK(output.keys[1].state == UMI_AI_TRAINING_TRUST_KEY_REVOKED);

    CHECK(RejectWithoutModification("key=" KEY_A ",a.pem,0,10,active\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-1\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-2\nkey=" KEY_A ",a.pem,0,10,active\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-1\nunknown=hello\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-1\nkey=ABCDEF,a.pem,0,10,active\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-1\nkey=" KEY_A ",../secret.pem,0,10,active\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-1\nkey=" KEY_A ",keys//a.pem,0,10,active\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-1\nkey=" KEY_A ",keys\\a.pem,0,10,active\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-1\nkey=" KEY_A ",C:secret.pem,0,10,active\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-1\nkey=" KEY_A ",a.pem,-1,10,active\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-1\nkey=" KEY_A ",a.pem,01,10,active\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-1\nkey=" KEY_A ",a.pem,20,20,active\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-1\nkey=" KEY_A ",a.pem,0,99999999999999999999999,active\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-1\nkey=" KEY_A ",a.pem,0,10,expired\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-1\nkey=" KEY_A ",a.pem,0,10,active,extra\n") == 0);
    CHECK(RejectWithoutModification("UMICOM-AI-TRUST-1\nkey=" KEY_A ",a.pem,0,10,active\nkey=" KEY_A ",b.pem,0,10,revoked\n") == 0);
    /* A short NUL embedded inside a purported policy may never truncate it
     * into a different, accepted policy. */
    {
        const char embedded[] =
            "UMICOM-AI-TRUST-1\nkey=" KEY_A ",a.pem,0,10,active\n\0"
            "key=" KEY_B ",b.pem,0,10,revoked\n";
        UmiAiTrainingTrustPolicy preserved;
        preserved = output;
        CHECK(UmiAiTrainingTrustPolicyParse(
            embedded, sizeof(embedded) - 1U, &output) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&output, &preserved, sizeof(output)) == 0);
    }
    {
        char oversized[UMI_AI_TRAINING_TRUST_MAX_FILE_BYTES + 2U];
        memset(oversized, '#', sizeof(oversized));
        CHECK(UmiAiTrainingTrustPolicyParse(
            oversized, sizeof(oversized), &output) ==
            UMI_STATUS_CAPACITY_EXCEEDED);
    }
    puts("Strict C23 training publisher trust-policy grammar: PASS");
    return 0;
}
