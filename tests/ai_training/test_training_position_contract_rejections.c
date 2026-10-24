/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_position_contract_rejections.c
 * PURPOSE: Protect all outputs on rejection, reject corrupted/forged sidecars
 *          and refuse unsupported positional strategies and context modes.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/position_contract.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; \
} } while (0)

static void ResealForCorruptionTest(uint8_t record[UMI_AI_POSITION_CONTRACT_RECORD_BYTES])
{
    /* An unsigned sidecar can be deliberately rehashed by an attacker.
     * Decoder-policy validation must still reject unsupported declarations.
     * This does NOT emulate a digital signature or verified publisher. */
    UmiSha256 sha;
    UmiSha256Init(&sha);
    (void)UmiSha256Update(&sha, record, 96U);
    (void)UmiSha256Final(&sha, record + 96U);
}

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(91)
    };
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingPositionContract original = {0}, unchanged, modified;
    uint8_t encoded[UMI_AI_POSITION_CONTRACT_RECORD_BYTES] = {0};
    uint8_t damaged[UMI_AI_POSITION_CONTRACT_RECORD_BYTES] = {0};
    uint8_t protectedBytes[UMI_AI_POSITION_CONTRACT_RECORD_BYTES];
    uint8_t expectedProtected[sizeof(protectedBytes)];
    unsigned char expectedContract[sizeof unchanged];
    size_t written = 999U;

    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractBuild(model,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED, &original) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractEncode(&original, encoded,
        sizeof(encoded), &written) == UMI_STATUS_OK);
    CHECK(written == sizeof(encoded));

    memset(&unchanged, 0xAB, sizeof unchanged);
    memcpy(expectedContract, &unchanged, sizeof unchanged);
    CHECK(UmiAiTrainingPositionContractBuild(NULL,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED,
        &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPositionContractBuild(model,
        (UmiAiTrainingPositionContext)999,
        &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPositionContractDecode(encoded,
        sizeof(encoded) - 1U, &unchanged) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiAiTrainingPositionContractDecode(encoded,
        sizeof(encoded) + 1U, &unchanged) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&unchanged, expectedContract, sizeof unchanged) == 0);

    memset(protectedBytes, 0xAC, sizeof protectedBytes);
    memcpy(expectedProtected, protectedBytes, sizeof protectedBytes);
    written = 12345U;
    CHECK(UmiAiTrainingPositionContractEncode(&original,
        protectedBytes, sizeof(protectedBytes) - 1U,
        &written) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(written == 12345U);
    CHECK(memcmp(protectedBytes, expectedProtected,
        sizeof protectedBytes) == 0);

    modified = original;
    modified.positionEncoding = 2U; /* Reserved for future positional model. */
    CHECK(UmiAiTrainingPositionContractEncode(&modified, protectedBytes,
        sizeof protectedBytes, &written) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(protectedBytes, expectedProtected, sizeof protectedBytes) == 0);
    modified = original;
    modified.rolloverBehaviour = 2U;
    CHECK(UmiAiTrainingPositionContractEncode(&modified, protectedBytes,
        sizeof protectedBytes, &written) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(protectedBytes, expectedProtected, sizeof protectedBytes) == 0);
    CHECK(UmiAiTrainingPositionContractVerify(model, &modified,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED) == UMI_STATUS_INVALID_STATE);

    memcpy(damaged, encoded, sizeof damaged);
    damaged[0U] ^= UINT8_C(0x01); /* Magic mismatch */
    CHECK(UmiAiTrainingPositionContractDecode(damaged,
        sizeof damaged, &unchanged) == UMI_STATUS_PARSE_ERROR);
    memcpy(damaged, encoded, sizeof damaged);
    damaged[75U] ^= UINT8_C(0x80); /* Checkpoint hash changed */
    CHECK(UmiAiTrainingPositionContractDecode(damaged,
        sizeof damaged, &unchanged) == UMI_STATUS_PARSE_ERROR);
    memcpy(damaged, encoded, sizeof damaged);
    damaged[120U] ^= UINT8_C(0x20); /* Record digest corrupted */
    CHECK(UmiAiTrainingPositionContractDecode(damaged,
        sizeof damaged, &unchanged) == UMI_STATUS_PARSE_ERROR);

    memcpy(damaged, encoded, sizeof damaged);
    damaged[12U] = UINT8_C(2); /* Unsupported future positional scheme */
    ResealForCorruptionTest(damaged);
    CHECK(UmiAiTrainingPositionContractDecode(damaged,
        sizeof damaged, &unchanged) == UMI_STATUS_PARSE_ERROR);
    memcpy(damaged, encoded, sizeof damaged);
    damaged[8U] = UINT8_C(2); /* Unsupported sidecar version */
    ResealForCorruptionTest(damaged);
    CHECK(UmiAiTrainingPositionContractDecode(damaged,
        sizeof damaged, &unchanged) == UMI_STATUS_PARSE_ERROR);
    memcpy(damaged, encoded, sizeof damaged);
    damaged[28U] = UINT8_C(0); /* Invalid context length */
    ResealForCorruptionTest(damaged);
    CHECK(UmiAiTrainingPositionContractDecode(damaged,
        sizeof damaged, &unchanged) == UMI_STATUS_PARSE_ERROR);

    CHECK(memcmp(&unchanged, expectedContract, sizeof unchanged) == 0);
    CHECK(UmiAiTrainingPositionContractVerify(model, &original,
        UMI_AI_POSITION_CONTEXT_BOS_LEFT_PADDED) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingPositionContractVerify(model, &original,
        (UmiAiTrainingPositionContext)3) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPositionContractVerify(model, &original,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED) == UMI_STATUS_OK);
    UmiAiTrainingDecoderDestroy(model);
    puts("position contract: malformed sidecar and output preservation PASS");
    return 0;
}
