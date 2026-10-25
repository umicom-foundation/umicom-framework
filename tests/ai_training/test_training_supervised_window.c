/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supervised_window.c
 * PURPOSE: Verify BOS/EOS, next-token alignment, prompt/completion boundaries,
 *          ignored padding, binary safety and failure-atomic output arrays.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/supervised_window.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "supervised window:%d: %s\n", __LINE__, #x); \
    return 1; } } while (0)

int main(void)
{
    const uint8_t prompt[] = {'H','i'};
    const uint8_t completion[] = {' ','O','K'};
    size_t inputs[8], targets[8];
    uint8_t mask[8];
    UmiAiTrainingSupervisedWindowInfo info = {0};
    static const size_t expectedInputs[8] = {
        UMI_AI_BYTE_BOS, 76U, 109U, 36U, 83U, 79U,
        UMI_AI_BYTE_EOS, UMI_AI_BYTE_PAD};
    static const size_t expectedTargets[8] = {
        76U, 109U, 36U, 83U, 79U, UMI_AI_BYTE_EOS,
        UMI_AI_BYTE_PAD, UMI_AI_BYTE_PAD};
    static const uint8_t expectedMask[8] = {0,0,1,1,1,1,0,0};

    CHECK(UmiAiTrainingSupervisedByteWindowBuild(prompt, 2U, completion, 3U,
        8U, inputs, 8U, targets, 8U, mask, 8U, &info) == UMI_STATUS_OK);
    CHECK(memcmp(inputs, expectedInputs, sizeof(inputs)) == 0);
    CHECK(memcmp(targets, expectedTargets, sizeof(targets)) == 0);
    CHECK(memcmp(mask, expectedMask, sizeof(mask)) == 0);
    CHECK(info.contextTokens == 8U && info.promptBytes == 2U &&
          info.completionBytes == 3U && info.supervisedTargets == 4U &&
          info.ignoredTargets == 4U);

    /* Exact-fit windows must supervise EOS without silently discarding it. */
    CHECK(UmiAiTrainingSupervisedByteWindowBuild(prompt, 2U, completion, 3U,
        6U, inputs, 8U, targets, 8U, mask, 8U, &info) == UMI_STATUS_OK);
    CHECK(targets[5] == UMI_AI_BYTE_EOS && mask[5] == 1U);

    /* An embedded zero is a byte token (ID 4), NOT an implicit string end. */
    const uint8_t binaryPrompt[] = {0U};
    const uint8_t binaryCompletion[] = {255U};
    CHECK(UmiAiTrainingSupervisedByteWindowBuild(
        binaryPrompt, 1U, binaryCompletion, 1U,
        3U, inputs, 8U, targets, 8U, mask, 8U, &info) == UMI_STATUS_OK);
    CHECK(inputs[1] == 4U && targets[1] == 259U &&
          targets[2] == UMI_AI_BYTE_EOS && mask[0] == 0U &&
          mask[1] == 1U && mask[2] == 1U);

    /* All failures leave the complete advertised output storage unchanged. */
    (void)memset(inputs, 0xA5, sizeof(inputs));
    (void)memset(targets, 0x5A, sizeof(targets));
    (void)memset(mask, 0xBD, sizeof(mask));
    UmiAiTrainingSupervisedWindowInfo oldInfo = info;
    const size_t priorInputs[8] = {inputs[0],inputs[1],inputs[2],inputs[3],
                                   inputs[4],inputs[5],inputs[6],inputs[7]};
    const size_t priorTargets[8] = {targets[0],targets[1],targets[2],targets[3],
                                    targets[4],targets[5],targets[6],targets[7]};
    uint8_t priorMask[8]; (void)memcpy(priorMask, mask, sizeof(mask));
    CHECK(UmiAiTrainingSupervisedByteWindowBuild(prompt, 2U, completion, 3U,
        5U, inputs, 8U, targets, 8U, mask, 8U, &info) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingSupervisedByteWindowBuild(prompt, 2U, completion, 3U,
        8U, inputs, 7U, targets, 8U, mask, 8U, &info) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingSupervisedByteWindowBuild(prompt, 2U, completion, 3U,
        8U, inputs, 8U, inputs, 8U, mask, 8U, &info) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(inputs, priorInputs, sizeof(inputs)) == 0);
    CHECK(memcmp(targets, priorTargets, sizeof(targets)) == 0);
    CHECK(memcmp(mask, priorMask, sizeof(mask)) == 0);
    CHECK(memcmp(&info, &oldInfo, sizeof(info)) == 0);
    puts("Reviewed supervised byte window: next-token alignment and rejection passed");
    return 0;
}
