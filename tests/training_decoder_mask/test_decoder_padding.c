/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_decoder_mask/test_decoder_padding.c
 *
 * PURPOSE:
 *   Verify padded decoder semantics, legacy parity, atomic refusal and repeated training lifetimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/checkpoint.h"
#include "../../src/ai/training/decoder_private.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static const UmiAiTrainingDecoderConfig cfg = {8, 3, 4, 8, 2, 42};
static const size_t ids[6] = {1, 2, 3, 4, 5, 6};
static const size_t targets[6] = {2, 3, 4, 5, 6, 7};
static const unsigned char attended[6] = {1, 0, 1, 0, 1, 1};
static const unsigned char scored[6] = {0, 0, 1, 0, 0, 1};
typedef struct Snapshot { unsigned char bytes[131072]; size_t count; } Snapshot;
static int save(UmiAiTrainingDecoder *model, Snapshot *snapshot)
{
    OK(UmiAiTrainingDecoderCheckpointEncode(model, snapshot->bytes,
        sizeof snapshot->bytes, &snapshot->count));
    return 0;
}
static int same(UmiAiTrainingDecoder *model, const Snapshot *snapshot)
{
    Snapshot after;
    CHECK(save(model, &after) == 0);
    CHECK(after.count == snapshot->count &&
        !memcmp(after.bytes, snapshot->bytes, after.count));
    return 0;
}
static int stable(UmiAiTrainingDecoder *model, const UmiAiTrainingTapeMemory *before)
{
    UmiAiTrainingTapeMemory after;
    OK(UmiAiTrainingDecoderMemoryInspect(model, &after));
/* Static header review found that the first fixture draft used nonexistent memory-report field names. The public contract names tensorCount and currentBytes; retain this draft comparison for review. The former implementation is retained for owner review. */
#if 0
    CHECK(after.nodeCount == before->nodeCount &&
        after.allocatedBytes == before->allocatedBytes &&
        after.auxiliaryBytes == before->auxiliaryBytes);
#endif
    CHECK(after.tensorCount == before->tensorCount &&
        after.currentBytes == before->currentBytes &&
        after.auxiliaryBytes == before->auxiliaryBytes);
    return 0;
}
static int reference(void)
{
    UmiAiTrainingDecoder *model = NULL;
    float output[48], value = -1;
    Snapshot initial;
    OK(UmiAiTrainingDecoderCreate(&cfg, &model));
    CHECK(save(model, &initial) == 0);
    OK(UmiAiTrainingDecoderLogitsPadded(model, ids, 6, attended, 6, output, 48));
    double sum = 0;
    for (size_t row = 0; row < 6; ++row) {
        if (!attended[row]) for (size_t col = 0; col < 8; ++col)
            CHECK(output[row * 8 + col] == 0);
        if (!scored[row]) continue;
        double maximum = output[row * 8], denominator = 0;
        for (size_t col = 1; col < 8; ++col)
            if (output[row * 8 + col] > maximum) maximum = output[row * 8 + col];
        for (size_t col = 0; col < 8; ++col)
            denominator += exp((double)output[row * 8 + col] - maximum);
        sum += (maximum - output[row * 8 + targets[row]]) + log(denominator);
    }
    OK(UmiAiTrainingDecoderEvaluatePadded(model, ids, targets, 6,
        attended, 6, scored, 6, &value));
    CHECK(fabs((double)value - sum / 2) < 2e-6);
    CHECK(same(model, &initial) == 0);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
static int invariance(int train)
{
    UmiAiTrainingDecoder *a = NULL, *b = NULL;
    const size_t changedIds[6] = {1, 7, 3, 0, 5, 6};
    const size_t changedTargets[6] = {7, 0, 4, 7, 1, 7};
    float oa[48], ob[48], la, lb;
    Snapshot state;
    OK(UmiAiTrainingDecoderCreate(&cfg, &a));
    OK(UmiAiTrainingDecoderCreate(&cfg, &b));
    for (size_t step = 0; step < 12; ++step) {
        OK(UmiAiTrainingDecoderLogitsPadded(a, ids, 6, attended, 6, oa, 48));
        OK(UmiAiTrainingDecoderLogitsPadded(b, changedIds, 6, attended, 6, ob, 48));
        CHECK(!memcmp(oa, ob, sizeof oa));
        if (train) {
            OK(UmiAiTrainingDecoderTrainPaddedStep(a, ids, targets, 6,
                attended, 6, scored, 6, 0.03F, &la));
            OK(UmiAiTrainingDecoderTrainPaddedStep(b, changedIds, changedTargets, 6,
                attended, 6, scored, 6, 0.03F, &lb));
        } else {
            OK(UmiAiTrainingDecoderEvaluatePadded(a, ids, targets, 6,
                attended, 6, scored, 6, &la));
            OK(UmiAiTrainingDecoderEvaluatePadded(b, changedIds, changedTargets, 6,
                attended, 6, scored, 6, &lb));
        }
        CHECK(la == lb);
        CHECK(save(a, &state) == 0 && same(b, &state) == 0);
    }
    UmiAiTrainingDecoderDestroy(a); UmiAiTrainingDecoderDestroy(b);
    return 0;
}
static int parity(void)
{
    UmiAiTrainingDecoder *a = NULL, *b = NULL;
    const unsigned char all[6] = {1, 1, 1, 1, 1, 1};
    float oa[48], ob[48], la, lb;
    Snapshot state;
    OK(UmiAiTrainingDecoderCreate(&cfg, &a));
    OK(UmiAiTrainingDecoderCreate(&cfg, &b));
    for (size_t step = 0; step < 8; ++step) {
        OK(UmiAiTrainingDecoderLogits(a, ids, 6, oa, 48));
        OK(UmiAiTrainingDecoderLogitsPadded(b, ids, 6, all, 6, ob, 48));
        CHECK(!memcmp(oa, ob, sizeof oa));
        OK(UmiAiTrainingDecoderTrainMaskedStep(a, ids, targets, 6,
            scored, 6, 0.03F, &la));
        OK(UmiAiTrainingDecoderTrainPaddedStep(b, ids, targets, 6,
            all, 6, scored, 6, 0.03F, &lb));
        CHECK(la == lb);
        CHECK(save(a, &state) == 0 && same(b, &state) == 0);
    }
    UmiAiTrainingDecoderDestroy(a); UmiAiTrainingDecoderDestroy(b);
    return 0;
}
static int empty(void)
{
    UmiAiTrainingDecoder *model = NULL;
    const unsigned char zero[6] = {0};
    float output[48], loss = -77;
    Snapshot state;
    OK(UmiAiTrainingDecoderCreate(&cfg, &model));
    CHECK(save(model, &state) == 0);
    OK(UmiAiTrainingDecoderLogitsPadded(model, ids, 6, zero, 6, output, 48));
    for (size_t i = 0; i < 48; ++i) CHECK(output[i] == 0);
    CHECK(UmiAiTrainingDecoderEvaluatePadded(model, ids, targets, 6,
        zero, 6, zero, 6, &loss) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(loss == -77 && same(model, &state) == 0);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
static int isolation(void)
{
    UmiAiTrainingDecoder *model = NULL;
    float a[48], b[48];
    size_t changed[6]; memcpy(changed, ids, sizeof ids);
    OK(UmiAiTrainingDecoderCreate(&cfg, &model));
    OK(UmiAiTrainingDecoderLogitsPadded(model, ids, 6, attended, 6, a, 48));
    changed[2] = 7;
    OK(UmiAiTrainingDecoderLogitsPadded(model, changed, 6, attended, 6, b, 48));
    CHECK(!memcmp(a, b, 16 * sizeof(float)));
    CHECK(!memcmp(a + 24, b + 24, 24 * sizeof(float)));
    CHECK(memcmp(a + 16, b + 16, 8 * sizeof(float)) != 0);
    changed[0] = 6; changed[1] = 7;
    OK(UmiAiTrainingDecoderLogitsPadded(model, changed, 6, attended, 6, b, 48));
    CHECK(!memcmp(a + 24, b + 24, 24 * sizeof(float)));
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
static int rejections(void)
{
    UmiAiTrainingDecoder *model = NULL;
    Snapshot state;
    UmiAiTrainingTapeMemory before;
    float loss = -77, output[48], untouched[48];
    for (size_t i = 0; i < 48; ++i) output[i] = untouched[i] = -77;
    OK(UmiAiTrainingDecoderCreate(&cfg, &model));
    CHECK(save(model, &state) == 0);
    OK(UmiAiTrainingDecoderMemoryInspect(model, &before));
    unsigned char badAttention[6], badScore[6];
    memcpy(badAttention, attended, 6); memcpy(badScore, scored, 6);
    badAttention[1] = 2; badScore[1] = 1;
    const unsigned char *masks[3] = {NULL, attended, badAttention};
    const size_t counts[3] = {6, 5, 6};
    for (size_t i = 0; i < 3; ++i) {
        CHECK(UmiAiTrainingDecoderLogitsPadded(model, ids, 6, masks[i], counts[i], output, 48) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingDecoderEvaluatePadded(model, ids, targets, 6, masks[i], counts[i], scored, 6, &loss) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingDecoderTrainPaddedStep(model, ids, targets, 6, masks[i], counts[i], scored, 6, 0.03F, &loss) == UMI_STATUS_INVALID_ARGUMENT);
    }
    CHECK(UmiAiTrainingDecoderTrainPaddedStep(model, ids, targets, 6, attended, 6, badScore, 6, 0.03F, &loss) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderEvaluatePadded(model, ids, targets, 6, attended, 6, NULL, 6, &loss) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderEvaluatePadded(model, ids, targets, 6, attended, 6, scored, SIZE_MAX, &loss) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderLogitsPadded(model, ids, 6, attended, 6, output, 47) == UMI_STATUS_CAPACITY_EXCEEDED);
    size_t invalid[6]; memcpy(invalid, ids, sizeof ids); invalid[1] = 8;
    CHECK(UmiAiTrainingDecoderLogitsPadded(model, invalid, 6, attended, 6, output, 48) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderEvaluatePadded(model, ids, invalid, 6, attended, 6, scored, 6, &loss) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderLogitsPadded(NULL, ids, 6, attended, 6, output, 48) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderLogitsPadded(model, ids, 0, attended, 0, output, 48) == UMI_STATUS_INVALID_ARGUMENT);
    const float rates[4] = {0, -1, NAN, INFINITY};
    for (size_t i = 0; i < 4; ++i)
        CHECK(UmiAiTrainingDecoderTrainPaddedStep(model, ids, targets, 6, attended, 6, scored, 6, rates[i], &loss) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(loss == -77 && !memcmp(output, untouched, sizeof output));
    CHECK(same(model, &state) == 0 && stable(model, &before) == 0);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
static int budget(void)
{
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingTapeMemory before;
    Snapshot state;
    float loss = -77, out[48], original[48];
    for (size_t i = 0; i < 48; ++i) out[i] = original[i] = -77;
    OK(UmiAiTrainingDecoderCreate(&cfg, &model));
    OK(UmiAiTrainingDecoderMemoryInspect(model, &before));
    CHECK(save(model, &state) == 0);
/* Use the public committed-byte field for the refusal budget; the draft spelling is retained for review. The former implementation is retained for owner review. */
#if 0
    OK(UmiAiTrainingDecoderSetMemoryBudget(model, before.allocatedBytes));
#endif
    OK(UmiAiTrainingDecoderSetMemoryBudget(model, before.currentBytes));
    CHECK(UmiAiTrainingDecoderLogitsPadded(model, ids, 6, attended, 6, out, 48) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingDecoderEvaluatePadded(model, ids, targets, 6, attended, 6, scored, 6, &loss) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingDecoderTrainPaddedStep(model, ids, targets, 6, attended, 6, scored, 6, 0.03F, &loss) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(loss == -77 && !memcmp(out, original, sizeof out));
    CHECK(same(model, &state) == 0 && stable(model, &before) == 0);
    OK(UmiAiTrainingDecoderSetMemoryBudget(model, 0));
    OK(UmiAiTrainingDecoderEvaluatePadded(model, ids, targets, 6, attended, 6, scored, 6, &loss));
    CHECK(stable(model, &before) == 0);
    UmiAiTrainingTapeMemory high;
    OK(UmiAiTrainingDecoderMemoryInspect(model, &high));
    const size_t limits[3] = {before.currentBytes + 1,
        before.currentBytes + (high.peakBytes - before.currentBytes) / 2,
        high.peakBytes - 1};
    for (size_t i = 0; i < 3; ++i) {
        OK(UmiAiTrainingDecoderSetMemoryBudget(model, limits[i]));
        loss = -77;
        CHECK(UmiAiTrainingDecoderTrainPaddedStep(model, ids, targets, 6, attended, 6, scored, 6, 0.03F, &loss) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(loss == -77 && same(model, &state) == 0 && stable(model, &before) == 0);
    }
    OK(UmiAiTrainingDecoderSetMemoryBudget(model, high.peakBytes));
    OK(UmiAiTrainingDecoderTrainPaddedStep(model, ids, targets, 6, attended, 6, scored, 6, 0.03F, &loss));
    CHECK(stable(model, &before) == 0);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
static int repeat(void)
{
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingTapeMemory before;
    float first, last, loss;
    OK(UmiAiTrainingDecoderCreate(&cfg, &model));
    OK(UmiAiTrainingDecoderMemoryInspect(model, &before));
    OK(UmiAiTrainingDecoderEvaluatePadded(model, ids, targets, 6, attended, 6, scored, 6, &first));
    for (size_t i = 0; i < 50; ++i) {
        OK(UmiAiTrainingDecoderTrainPaddedStep(model, ids, targets, 6, attended, 6, scored, 6, 0.03F, &loss));
        CHECK(stable(model, &before) == 0);
    }
    OK(UmiAiTrainingDecoderEvaluatePadded(model, ids, targets, 6, attended, 6, scored, 6, &last));
    CHECK(last < first && model->completedSteps == 50);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
static int resume(void)
{
    UmiAiTrainingDecoder *a = NULL, *b = NULL;
    Snapshot state;
    float la, lb;
    OK(UmiAiTrainingDecoderCreate(&cfg, &a));
    OK(UmiAiTrainingDecoderTrainPaddedStep(a, ids, targets, 6, attended, 6, scored, 6, 0.03F, &la));
    CHECK(save(a, &state) == 0);
    OK(UmiAiTrainingDecoderCheckpointDecode(state.bytes, state.count, &b));
    OK(UmiAiTrainingDecoderTrainPaddedStep(a, ids, targets, 6, attended, 6, scored, 6, 0.03F, &la));
    OK(UmiAiTrainingDecoderTrainPaddedStep(b, ids, targets, 6, attended, 6, scored, 6, 0.03F, &lb));
    CHECK(la == lb && save(a, &state) == 0 && same(b, &state) == 0);
    UmiAiTrainingDecoderDestroy(a); UmiAiTrainingDecoderDestroy(b);
    return 0;
}
static int overflow(void)
{
    UmiAiTrainingDecoder *model = NULL;
    Snapshot state;
    float loss = -77;
    OK(UmiAiTrainingDecoderCreate(&cfg, &model));
    model->completedSteps = UINT64_MAX;
    CHECK(save(model, &state) == 0);
    CHECK(UmiAiTrainingDecoderTrainPaddedStep(model, ids, targets, 6, attended, 6, scored, 6, 0.03F, &loss) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(loss == -77 && same(model, &state) == 0);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!strcmp(argv[1], "reference")) return reference();
    if (!strcmp(argv[1], "padding-invariance")) return invariance(0);
    if (!strcmp(argv[1], "training-invariance")) return invariance(1);
    if (!strcmp(argv[1], "legacy-parity")) return parity();
    if (!strcmp(argv[1], "empty")) return empty();
    if (!strcmp(argv[1], "isolation")) return isolation();
    if (!strcmp(argv[1], "rejections")) return rejections();
    if (!strcmp(argv[1], "budget")) return budget();
    if (!strcmp(argv[1], "repeat")) return repeat();
    if (!strcmp(argv[1], "resume")) return resume();
    if (!strcmp(argv[1], "step-overflow")) return overflow();
    return 2;
}
