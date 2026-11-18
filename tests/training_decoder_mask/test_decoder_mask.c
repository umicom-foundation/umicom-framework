/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_decoder_mask/test_decoder_mask.c
 *
 * PURPOSE:
 *   Verify selected decoder objectives, causal context, legacy parity and failure-atomic model state.
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

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static const UmiAiTrainingDecoderConfig config = {8U, 3U, 4U, 8U, 2U, 42U};
static const size_t inputs[6] = {1U, 2U, 3U, 4U, 5U, 6U};
static const size_t targets[6] = {2U, 3U, 4U, 5U, 6U, 7U};
static const unsigned char selection[6] = {0U, 0U, 1U, 0U, 1U, 1U};
typedef struct Snapshot { unsigned char bytes[131072]; size_t count; } Snapshot;
static int Save(UmiAiTrainingDecoder *model, Snapshot *snapshot)
{
    OK(UmiAiTrainingDecoderCheckpointEncode(model, snapshot->bytes,
        sizeof(snapshot->bytes), &snapshot->count));
    return 0;
}
static int Same(UmiAiTrainingDecoder *model, const Snapshot *snapshot)
{
    Snapshot after;
    CHECK(Save(model, &after) == 0);
    CHECK(after.count == snapshot->count);
    CHECK(memcmp(after.bytes, snapshot->bytes, after.count) == 0);
    return 0;
}
static int Reference(void)
{
    UmiAiTrainingDecoder *model = NULL;
    float logits[48], loss = -1.0F;
    Snapshot before;
    OK(UmiAiTrainingDecoderCreate(&config, &model));
    CHECK(Save(model, &before) == 0);
    OK(UmiAiTrainingDecoderLogits(model, inputs, 6U, logits, 48U));
    double sum = 0.0;
    size_t active = 0U;
    for (size_t i = 0U; i < 6U; ++i) {
        if (!selection[i]) continue;
        double maximum = logits[i * 8U];
        for (size_t c = 1U; c < 8U; ++c)
            if ((double)logits[i * 8U + c] > maximum) maximum = logits[i * 8U + c];
        double denominator = 0.0;
        for (size_t c = 0U; c < 8U; ++c)
            denominator += exp((double)logits[i * 8U + c] - maximum);
        sum += (maximum - logits[i * 8U + targets[i]]) + log(denominator);
        ++active;
    }
    OK(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 6U,
        selection, 6U, &loss));
    CHECK(fabs((double)loss - sum / (double)active) < 2e-6);
    CHECK(Same(model, &before) == 0);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
static int FullMaskParity(void)
{
    UmiAiTrainingDecoder *left = NULL, *right = NULL;
    const unsigned char all[6] = {1U, 1U, 1U, 1U, 1U, 1U};
    float a = 0.0F, b = 0.0F;
    Snapshot snapshot;
    OK(UmiAiTrainingDecoderCreate(&config, &left));
    OK(UmiAiTrainingDecoderCreate(&config, &right));
    for (size_t step = 0U; step < 12U; ++step) {
        OK(UmiAiTrainingDecoderEvaluate(left, inputs, targets, 6U, &a));
        OK(UmiAiTrainingDecoderEvaluateMasked(right, inputs, targets, 6U, all, 6U, &b));
        CHECK(memcmp(&a, &b, sizeof(a)) == 0);
        OK(UmiAiTrainingDecoderTrainStep(left, inputs, targets, 6U, 0.03F, &a));
        OK(UmiAiTrainingDecoderTrainMaskedStep(right, inputs, targets, 6U, all, 6U, 0.03F, &b));
        CHECK(memcmp(&a, &b, sizeof(a)) == 0);
        CHECK(Save(left, &snapshot) == 0 && Same(right, &snapshot) == 0);
    }
    UmiAiTrainingDecoderDestroy(left);
    UmiAiTrainingDecoderDestroy(right);
    return 0;
}
static int ExcludedTargets(void)
{
    UmiAiTrainingDecoder *a = NULL, *b = NULL, *c = NULL;
    const size_t other[6] = {7U, 0U, 4U, 2U, 6U, 7U};
    const size_t changed[6] = {2U, 3U, 5U, 5U, 6U, 7U};
    float la = 0.0F, lb = 0.0F, lc = 0.0F;
    Snapshot sa, sc;
    OK(UmiAiTrainingDecoderCreate(&config, &a));
    OK(UmiAiTrainingDecoderCreate(&config, &b));
    OK(UmiAiTrainingDecoderCreate(&config, &c));
    OK(UmiAiTrainingDecoderTrainMaskedStep(a, inputs, targets, 6U, selection, 6U, 0.1F, &la));
    OK(UmiAiTrainingDecoderTrainMaskedStep(b, inputs, other, 6U, selection, 6U, 0.1F, &lb));
    OK(UmiAiTrainingDecoderTrainMaskedStep(c, inputs, changed, 6U, selection, 6U, 0.1F, &lc));
    CHECK(memcmp(&la, &lb, sizeof(la)) == 0);
    CHECK(Save(a, &sa) == 0 && Same(b, &sa) == 0 && Save(c, &sc) == 0);
    CHECK(sa.count == sc.count && memcmp(sa.bytes, sc.bytes, sa.count) != 0);
    UmiAiTrainingDecoderDestroy(a);
    UmiAiTrainingDecoderDestroy(b);
    UmiAiTrainingDecoderDestroy(c);
    return 0;
}
static int Rejections(void)
{
    UmiAiTrainingDecoder *model = NULL;
    float loss = 123.0F;
    unsigned char bad[6] = {0U};
    size_t invalidTargets[6];
    UmiAiTrainingTapeMemory base, after;
    Snapshot before;
    OK(UmiAiTrainingDecoderCreate(&config, &model));
    OK(UmiAiTrainingDecoderMemoryInspect(model, &base));
    CHECK(Save(model, &before) == 0);
#define REFUSE(call) do { CHECK((call) == UMI_STATUS_INVALID_ARGUMENT); CHECK(loss == 123.0F); CHECK(Same(model, &before) == 0); } while (0)
    REFUSE(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 6U, NULL, 6U, &loss));
    REFUSE(UmiAiTrainingDecoderTrainMaskedStep(model, inputs, targets, 6U, NULL, 6U, 0.1F, &loss));
    REFUSE(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 6U, selection, 5U, &loss));
    REFUSE(UmiAiTrainingDecoderTrainMaskedStep(model, inputs, targets, 6U, selection, 7U, 0.1F, &loss));
    REFUSE(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 6U, bad, 6U, &loss));
    bad[5] = 2U;
    REFUSE(UmiAiTrainingDecoderTrainMaskedStep(model, inputs, targets, 6U, bad, 6U, 0.1F, &loss));
    memcpy(invalidTargets, targets, sizeof(targets)); invalidTargets[0] = 8U;
    REFUSE(UmiAiTrainingDecoderEvaluateMasked(model, inputs, invalidTargets, 6U, selection, 6U, &loss));
    REFUSE(UmiAiTrainingDecoderTrainMaskedStep(model, inputs, targets, 6U, selection, 6U, NAN, &loss));
    REFUSE(UmiAiTrainingDecoderTrainMaskedStep(model, inputs, targets, 6U, selection, 6U, 0.0F, &loss));
    REFUSE(UmiAiTrainingDecoderTrainMaskedStep(model, inputs, targets, 6U, selection, 6U, -0.1F, &loss));
    REFUSE(UmiAiTrainingDecoderTrainMaskedStep(model, inputs, targets, 6U, selection, 6U, INFINITY, &loss));
    REFUSE(UmiAiTrainingDecoderEvaluateMasked(NULL, inputs, targets, 6U, selection, 6U, &loss));
    REFUSE(UmiAiTrainingDecoderEvaluateMasked(model, NULL, targets, 6U, selection, 6U, &loss));
    REFUSE(UmiAiTrainingDecoderEvaluateMasked(model, inputs, NULL, 6U, selection, 6U, &loss));
    REFUSE(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 5U, selection, 5U, &loss));
    REFUSE(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 0U, selection, 0U, &loss));
    REFUSE(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 6U, selection, 6U, NULL));
#undef REFUSE
    OK(UmiAiTrainingDecoderMemoryInspect(model, &after));
    CHECK(after.currentBytes == base.currentBytes && after.peakBytes == base.peakBytes);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
static int BudgetRollback(void)
{
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingTapeMemory base, high, after;
    Snapshot before;
    float loss = 0.0F;
    OK(UmiAiTrainingDecoderCreate(&config, &model));
    OK(UmiAiTrainingDecoderMemoryInspect(model, &base));
    OK(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 6U, selection, 6U, &loss));
    OK(UmiAiTrainingDecoderMemoryInspect(model, &high));
    CHECK(high.peakBytes > base.currentBytes);
    CHECK(Save(model, &before) == 0);
    const size_t limits[3] = {base.currentBytes + 1U,
        base.currentBytes + (high.peakBytes - base.currentBytes) / 2U, high.peakBytes - 1U};
    for (size_t i = 0U; i < 3U; ++i) {
        OK(UmiAiTrainingDecoderSetMemoryBudget(model, limits[i]));
        loss = 123.0F;
        CHECK(UmiAiTrainingDecoderTrainMaskedStep(model, inputs, targets, 6U,
            selection, 6U, 0.1F, &loss) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(loss == 123.0F && Same(model, &before) == 0);
        OK(UmiAiTrainingDecoderMemoryInspect(model, &after));
        CHECK(after.currentBytes == base.currentBytes && after.tensorCount == base.tensorCount);
    }
    OK(UmiAiTrainingDecoderSetMemoryBudget(model, 0U));
    OK(UmiAiTrainingDecoderTrainMaskedStep(model, inputs, targets, 6U, selection, 6U, 0.1F, &loss));
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
static int RepeatedSteps(void)
{
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingTapeMemory base, after;
    float initial = 0.0F, loss = 0.0F;
    uint64_t steps = 0U;
    UmiAiTrainingDecoderConfig observed;
    OK(UmiAiTrainingDecoderCreate(&config, &model));
    OK(UmiAiTrainingDecoderMemoryInspect(model, &base));
    OK(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 6U, selection, 6U, &initial));
    size_t peak = 0U;
    for (size_t i = 0U; i < 200U; ++i) {
        OK(UmiAiTrainingDecoderTrainMaskedStep(model, inputs, targets, 6U, selection, 6U, 0.05F, &loss));
        OK(UmiAiTrainingDecoderMemoryInspect(model, &after));
        CHECK(after.currentBytes == base.currentBytes && after.tensorCount == base.tensorCount);
        if (i == 0U) peak = after.peakBytes;
        else CHECK(after.peakBytes == peak);
    }
    OK(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 6U, selection, 6U, &loss));
    CHECK(loss < initial);
    OK(UmiAiTrainingDecoderInspect(model, &observed, &steps));
    CHECK(steps == 200U);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
static int ContextIsNotMasked(void)
{
    UmiAiTrainingDecoder *model = NULL;
    const size_t prefixChanged[3] = {7U, 2U, 3U};
    const size_t futureChanged[3] = {1U, 2U, 7U};
    const unsigned char last[3] = {0U, 0U, 1U}, first[3] = {1U, 0U, 0U};
    float a = 0.0F, b = 0.0F;
    OK(UmiAiTrainingDecoderCreate(&config, &model));
    OK(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 3U, last, 3U, &a));
    OK(UmiAiTrainingDecoderEvaluateMasked(model, prefixChanged, targets, 3U, last, 3U, &b));
    CHECK(fabsf(a - b) > 1e-6F);
    OK(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 3U, first, 3U, &a));
    OK(UmiAiTrainingDecoderEvaluateMasked(model, futureChanged, targets, 3U, first, 3U, &b));
    CHECK(memcmp(&a, &b, sizeof(a)) == 0);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
static int Resume(void)
{
    UmiAiTrainingDecoder *original = NULL, *restored = NULL;
    float a = 0.0F, b = 0.0F;
    Snapshot snapshot;
    OK(UmiAiTrainingDecoderCreate(&config, &original));
    for (size_t i = 0U; i < 5U; ++i)
        OK(UmiAiTrainingDecoderTrainMaskedStep(original, inputs, targets, 6U, selection, 6U, 0.03F, &a));
    CHECK(Save(original, &snapshot) == 0);
    OK(UmiAiTrainingDecoderCheckpointDecode(snapshot.bytes, snapshot.count, &restored));
    for (size_t i = 0U; i < 5U; ++i) {
        OK(UmiAiTrainingDecoderTrainMaskedStep(original, inputs, targets, 6U, selection, 6U, 0.03F, &a));
        OK(UmiAiTrainingDecoderTrainMaskedStep(restored, inputs, targets, 6U, selection, 6U, 0.03F, &b));
        CHECK(memcmp(&a, &b, sizeof(a)) == 0);
        CHECK(Save(original, &snapshot) == 0 && Same(restored, &snapshot) == 0);
    }
    UmiAiTrainingDecoderDestroy(original);
    UmiAiTrainingDecoderDestroy(restored);
    return 0;
}
static int BatchWeighting(void)
{
    UmiAiTrainingDecoder *model = NULL;
    float batch = 0.0F, left = 0.0F, right = 0.0F;
    OK(UmiAiTrainingDecoderCreate(&config, &model));
    OK(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 6U, selection, 6U, &batch));
    OK(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 3U, selection, 3U, &left));
    OK(UmiAiTrainingDecoderEvaluateMasked(model, inputs + 3U, targets + 3U, 3U, selection + 3U, 3U, &right));
    CHECK(fabs((double)batch - ((double)left + 2.0 * (double)right) / 3.0) < 2e-6);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
static int StepOverflow(void)
{
    UmiAiTrainingDecoder *model = NULL;
    Snapshot before;
    float loss = 123.0F;
    OK(UmiAiTrainingDecoderCreate(&config, &model));
    /* Private test-only fixture reaches a counter boundary without billions of steps. */
    model->completedSteps = UINT64_MAX;
    CHECK(Save(model, &before) == 0);
    CHECK(UmiAiTrainingDecoderTrainMaskedStep(model, inputs, targets, 6U,
        selection, 6U, 0.1F, &loss) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(loss == 123.0F && Same(model, &before) == 0);
    OK(UmiAiTrainingDecoderEvaluateMasked(model, inputs, targets, 6U, selection, 6U, &loss));
    CHECK(Same(model, &before) == 0);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    struct Case { const char *name; int (*run)(void); } cases[] = {
        {"reference", Reference}, {"full-mask-parity", FullMaskParity},
        {"excluded-targets", ExcludedTargets}, {"rejections", Rejections},
        {"budget-rollback", BudgetRollback}, {"repeated-steps", RepeatedSteps},
        {"causal-context", ContextIsNotMasked}, {"checkpoint-resume", Resume},
        {"batch-weighting", BatchWeighting}, {"step-overflow", StepOverflow}
    };
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(argv[1], cases[i].name) == 0) return cases[i].run();
    return 2;
}
