/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_position_contract_roundtrip.c
 * PURPOSE: Bind both explicit context policies to one REAL checkpoint, verify
 *          portable sidecar roundtrip and independently decoded model parity.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/position_contract.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; \
} } while (0)

static int SameContract(const UmiAiTrainingPositionContract *left,
                        const UmiAiTrainingPositionContract *right)
{
    return left->formatVersion == right->formatVersion &&
        left->positionEncoding == right->positionEncoding &&
        left->generationContext == right->generationContext &&
        left->rolloverBehaviour == right->rolloverBehaviour &&
        left->decoderConfig.vocabularySize == right->decoderConfig.vocabularySize &&
        left->decoderConfig.contextLength == right->decoderConfig.contextLength &&
        left->decoderConfig.channels == right->decoderConfig.channels &&
        left->decoderConfig.hiddenChannels == right->decoderConfig.hiddenChannels &&
        left->decoderConfig.attentionHeads == right->decoderConfig.attentionHeads &&
        left->decoderConfig.initialSeed == right->decoderConfig.initialSeed &&
        left->completedTrainingSteps == right->completedTrainingSteps &&
        left->checkpointBytes == right->checkpointBytes &&
        memcmp(left->checkpointSha256, right->checkpointSha256,
               UMI_SHA256_BYTES) == 0;
}

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261024)
    };
    const UmiAiTrainingDecoderConfig otherConfig = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261025)
    };
    UmiAiTrainingDecoder *model = NULL, *restored = NULL, *different = NULL;
    UmiAiTrainingPositionContract legacy = {0}, prefix = {0}, decoded = {0};
    uint8_t sidecar[UMI_AI_POSITION_CONTRACT_RECORD_BYTES] = {0};
    uint8_t sidecarAgain[UMI_AI_POSITION_CONTRACT_RECORD_BYTES] = {0};
    uint8_t *checkpoint = NULL;
    size_t checkpointSize = 0U, written = 0U, encoded = 0U;

    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractBuild(model,
        UMI_AI_POSITION_CONTEXT_BOS_LEFT_PADDED, &legacy) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractBuild(model,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED, &prefix) == UMI_STATUS_OK);
    CHECK(legacy.formatVersion == UMI_AI_POSITION_CONTRACT_FORMAT);
    CHECK(legacy.positionEncoding == UMI_AI_POSITION_ENCODING_LEARNED_ABSOLUTE);
    CHECK(legacy.rolloverBehaviour == UMI_AI_POSITION_ROLLOVER_REBUILD);
    CHECK(prefix.decoderConfig.initialSeed == config.initialSeed);
    CHECK(legacy.generationContext != prefix.generationContext);
    /* Context modes differ, but are bound to exactly the same MODEL bytes. */
    CHECK(memcmp(legacy.checkpointSha256, prefix.checkpointSha256,
                 UMI_SHA256_BYTES) == 0);
    CHECK(UmiAiTrainingPositionContractVerify(model, &legacy,
        UMI_AI_POSITION_CONTEXT_BOS_LEFT_PADDED) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractVerify(model, &prefix,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractVerify(model, &legacy,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED) == UMI_STATUS_INVALID_STATE);

    CHECK(UmiAiTrainingPositionContractEncode(&prefix,
        sidecar, sizeof(sidecar), &encoded) == UMI_STATUS_OK);
    CHECK(encoded == UMI_AI_POSITION_CONTRACT_RECORD_BYTES);
    CHECK(memcmp(sidecar, "UMIPOS01", 8U) == 0);
    CHECK(UmiAiTrainingPositionContractDecode(sidecar,
        sizeof(sidecar), &decoded) == UMI_STATUS_OK);
    CHECK(SameContract(&prefix, &decoded));
    CHECK(UmiAiTrainingPositionContractEncode(&decoded,
        sidecarAgain, sizeof(sidecarAgain), &written) == UMI_STATUS_OK);
    CHECK(written == sizeof(sidecarAgain));
    CHECK(memcmp(sidecar, sidecarAgain, sizeof(sidecar)) == 0);

    /* The ORIGINAL checkpoint, not a reconstructed approximation, is used
     * to qualify interoperability with an independently decoded model. */
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &checkpointSize) == UMI_STATUS_OK);
    CHECK(checkpointSize == (size_t)prefix.checkpointBytes);
    checkpoint = malloc(checkpointSize);
    CHECK(checkpoint != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, checkpoint,
        checkpointSize, &written) == UMI_STATUS_OK);
    CHECK(written == checkpointSize);
    CHECK(UmiAiTrainingDecoderCheckpointDecode(checkpoint,
        checkpointSize, &restored) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractVerify(restored, &decoded,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED) == UMI_STATUS_OK);

    CHECK(UmiAiTrainingDecoderCreate(&otherConfig, &different) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractVerify(different, &decoded,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingPositionContractVerify(model, &decoded,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED) == UMI_STATUS_OK);
    UmiAiTrainingDecoderDestroy(different);
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingDecoderDestroy(model);
    free(checkpoint);
    puts("position contract: canonical checkpoint and explicit policy parity PASS");
    return 0;
}
