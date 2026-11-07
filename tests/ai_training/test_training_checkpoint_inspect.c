/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_checkpoint_inspect.c
 *
 * PURPOSE:
 *   Verify complete checkpoint inspection, decoder parity and failure-atomic metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/checkpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)

/* A small independent byte fixture helper recomputes the format checksum.
 * This lets shape/value rejection tests reach the parser after integrity
 * succeeds, instead of accidentally testing checksum rejection every time. */
static void Write32(uint8_t *bytes, size_t at, uint32_t value)
{
    for (size_t i = 0U; i < 4U; ++i) bytes[at + i] = (uint8_t)(value >> (8U * i));
}
static void RepairChecksum(uint8_t *bytes, size_t length)
{
    uint64_t value = UINT64_C(14695981039346656037);
    for (size_t i = 0U; i < length - 8U; ++i) {
        value ^= bytes[i];
        value *= UINT64_C(1099511628211);
    }
    for (size_t i = 0U; i < 8U; ++i)
        bytes[length - 8U + i] = (uint8_t)(value >> (8U * i));
}
static int Reject(const uint8_t *bytes, size_t length, UmiStatus expected,
                  UmiAiTrainingDecoder *unchanged)
{
    UmiAiTrainingDecoderCheckpointInfo info;
    unsigned char before[sizeof(info)];
    UmiAiTrainingDecoder *model = unchanged;
    memset(&info, 0xA5, sizeof(info));
    memcpy(before, &info, sizeof(info));
    CHECK(UmiAiTrainingDecoderCheckpointInspect(bytes, length, &info) == expected);
    CHECK(memcmp(before, &info, sizeof(info)) == 0);
    CHECK(UmiAiTrainingDecoderCheckpointDecode(bytes, length, &model) == expected);
    CHECK(model == unchanged);
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *scenario = argv[1];
    const UmiAiTrainingDecoderConfig config = {4U, 2U, 4U, 8U, 2U, UINT64_C(13)};
    UmiAiTrainingDecoder *model = NULL, *decoded = NULL;
    UmiAiTrainingDecoderCheckpointInfo info;
    size_t length = 0U, written = 0U;
    uint8_t *bytes;
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &length) == UMI_STATUS_OK);
    bytes = malloc(length + 1U);
    CHECK(bytes != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, bytes, length, &written) == UMI_STATUS_OK);
    CHECK(written == length);
    if (strcmp(scenario, "metadata") == 0) {
        CHECK(UmiAiTrainingDecoderCheckpointInspect(bytes, length, &info) == UMI_STATUS_OK);
        CHECK(info.config.vocabularySize == config.vocabularySize);
        CHECK(info.config.contextLength == config.contextLength);
        CHECK(info.config.channels == config.channels);
        CHECK(info.config.hiddenChannels == config.hiddenChannels);
        CHECK(info.config.attentionHeads == config.attentionHeads);
        CHECK(info.config.initialSeed == config.initialSeed);
        CHECK(info.completedSteps == 0U && info.formatVersion == 1U);
        CHECK(info.tensorCount == UMI_AI_DECODER_MAX_PARAMETERS);
        CHECK(info.byteCount == length);
        CHECK(52U + info.tensorCount * 8U + info.weightCount * 4U + 8U == length);
        CHECK(UmiAiTrainingDecoderCheckpointDecode(bytes, length, &decoded) == UMI_STATUS_OK);
        UmiAiTrainingDecoderConfig loaded;
        uint64_t steps = UINT64_MAX;
        CHECK(UmiAiTrainingDecoderInspect(decoded, &loaded, &steps) == UMI_STATUS_OK);
        CHECK(loaded.initialSeed == info.config.initialSeed && steps == info.completedSteps);
    } else if (strcmp(scenario, "trained") == 0) {
        size_t input[] = {0U, 1U}, target[] = {1U, 2U};
        float loss;
        CHECK(UmiAiTrainingDecoderTrainStep(model, input, target, 2U, 0.1F, &loss) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingDecoderCheckpointEncode(model, bytes, length, &written) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingDecoderCheckpointInspect(bytes, length, &info) == UMI_STATUS_OK);
        CHECK(info.completedSteps == 1U);
    } else if (strcmp(scenario, "null-input") == 0) {
        CHECK(Reject(NULL, length, UMI_STATUS_INVALID_ARGUMENT, model) == 0);
    } else if (strcmp(scenario, "null-output") == 0) {
        CHECK(UmiAiTrainingDecoderCheckpointInspect(bytes, length, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingDecoderCheckpointDecode(bytes, length, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(scenario, "truncated") == 0) {
        for (size_t i = 0U; i < length; ++i)
            CHECK(Reject(bytes, i, UMI_STATUS_PARSE_ERROR, model) == 0);
    } else if (strcmp(scenario, "trailing") == 0) {
        bytes[length] = 0U;
        CHECK(Reject(bytes, length + 1U, UMI_STATUS_PARSE_ERROR, model) == 0);
    } else if (strcmp(scenario, "oversized") == 0) {
        /* The public size guard must reject before reading this short buffer. */
        CHECK(Reject(bytes, UMI_AI_DECODER_MAX_CHECKPOINT_BYTES + 1U,
                     UMI_STATUS_PARSE_ERROR, model) == 0);
    } else {
        if (strcmp(scenario, "magic") == 0) bytes[0] ^= 1U;
        else if (strcmp(scenario, "version") == 0) Write32(bytes, 8U, 2U);
        else if (strcmp(scenario, "configuration") == 0) Write32(bytes, 12U, UINT32_MAX);
        else if (strcmp(scenario, "tensor-count") == 0) Write32(bytes, 48U, 0U);
        else if (strcmp(scenario, "shape") == 0) Write32(bytes, 52U, UINT32_MAX);
        else if (strcmp(scenario, "nan") == 0) Write32(bytes, 60U, UINT32_C(0x7fc00000));
        else if (strcmp(scenario, "infinity") == 0) Write32(bytes, 60U, UINT32_C(0x7f800000));
        else if (strcmp(scenario, "checksum") == 0) bytes[length - 1U] ^= 1U;
        else { fprintf(stderr, "unknown scenario: %s\n", scenario); return 1; }
        if (strcmp(scenario, "checksum") != 0) RepairChecksum(bytes, length);
        CHECK(Reject(bytes, length, UMI_STATUS_PARSE_ERROR, model) == 0);
    }
    UmiAiTrainingDecoderDestroy(decoded);
    UmiAiTrainingDecoderDestroy(model);
    free(bytes);
    return 0;
}
