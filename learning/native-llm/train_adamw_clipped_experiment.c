/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-llm/train_adamw_clipped_experiment.c
 *
 * PURPOSE:
 *   Thin C23 educational CLI for an explicitly reviewed global-gradient
 *   clipping policy. The Framework owns training, optimiser and provenance.
 *
 * SAFETY:
 *   A clipped checkpoint can be resumed only with a matching immutable
 *   companion record. Combined checkpoint bytes are never altered to store
 *   metadata. New checkpoint/record publication refuses existing paths.
 *   Only approved local plans, source datasets and checkpoints should be used.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment_adamw.h"
#include "umicom/ai/training/experiment_plan.h"
#include "umicom/ai/training/gradient_policy.h"
#include "umicom/ai/training/optimizer_checkpoint.h"
#include "umicom/ai/training/training_run_lock.h"
#include "umicom/ai/training/training_source_lock.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_CLIPPED_EXPERIMENT_PATH_MAX 1024U
#define UMI_CLIPPED_EXPERIMENT_POLICY_SUFFIX ".clip-policy"

static int ReadUnsigned(const char *text, uint64_t *outNumber)
{
    uint64_t value = 0U;
    if (text == NULL || text[0] == '\0' || outNumber == NULL) return 0;
    for (const char *cursor = text; *cursor != '\0'; ++cursor) {
        uint64_t digit;
        if (*cursor < '0' || *cursor > '9') return 0;
        digit = (uint64_t)(unsigned)(*cursor - '0');
        if (value > (UINT64_MAX - digit) / UINT64_C(10)) return 0;
        value = value * UINT64_C(10) + digit;
    }
    if (value == 0U) return 0;
    *outNumber = value;
    return 1;
}

static UmiStatus Progress(const UmiAiTrainingExperimentProgress *step,
                          void *context)
{
    (void)context;
    if (step == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    printf("step %" PRIu64 " | held-out CE %.6f | perplexity %.6f\n",
           step->completedSteps, step->heldOutCrossEntropy,
           step->heldOutPerplexity);
    return UMI_STATUS_OK;
}

static UmiStatus EncodePair(const UmiAiTrainingDecoder *model,
                            const UmiAiTrainingAdamW *optimizer,
                            uint8_t **outBytes, size_t *outCount)
{
    uint8_t *bytes = NULL;
    size_t capacity = 0U, written = 0U;
    UmiStatus status;
    if (model == NULL || optimizer == NULL ||
        outBytes == NULL || outCount == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingAdamWCheckpointSize(model, optimizer, &capacity);
    if (status != UMI_STATUS_OK) return status;
    if (capacity == 0U || capacity > UMI_AI_ADAMW_MAX_CHECKPOINT_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    bytes = malloc(capacity);
    if (bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiTrainingAdamWCheckpointEncode(model, optimizer,
                                                 bytes, capacity, &written);
    if (status == UMI_STATUS_OK && written != capacity)
        status = UMI_STATUS_INVALID_STATE;
    if (status != UMI_STATUS_OK) {
        free(bytes);
        return status;
    }
    *outBytes = bytes;
    *outCount = written;
    return UMI_STATUS_OK;
}

/* Only create the suffix in a bounded caller buffer. The provenance file
 * itself is emitted exclusively through the reusable Framework C23 service. */
static UmiStatus CompanionPath(const char *checkpointPath,
                                char *outPath, size_t capacity)
{
    size_t nameLength, suffixLength;
    if (checkpointPath == NULL || checkpointPath[0] == '\0' ||
        outPath == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    nameLength = strlen(checkpointPath);
    suffixLength = sizeof(UMI_CLIPPED_EXPERIMENT_POLICY_SUFFIX) - 1U;
    if (nameLength >= UMI_CLIPPED_EXPERIMENT_PATH_MAX ||
        nameLength > SIZE_MAX - suffixLength - 1U ||
        nameLength + suffixLength + 1U > capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outPath, checkpointPath, nameLength);
    memcpy(outPath + nameLength, UMI_CLIPPED_EXPERIMENT_POLICY_SUFFIX,
           suffixLength + 1U);
    return UMI_STATUS_OK;
}

/* Optional stronger continuation evidence: the established command line
 * remains untouched unless the user explicitly supplies this final flag.
 * The added run lock binds the dataset catalogue and original training
 * schedule, while retaining the independently checked .clip-policy record. */
/* Source-byte identity is an independently verified optional companion.
 * The legacy policy and run-lock paths are retained unchanged. */
static UmiStatus SourceLockPath(const char *checkpointPath,
                                char *outPath, size_t capacity)
{
    const char suffix[] = ".source-lock";
    size_t pathLength, suffixLength = sizeof(suffix) - 1U;
    if (checkpointPath == NULL || checkpointPath[0] == '\0' ||
        outPath == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    pathLength = strlen(checkpointPath);
    if (pathLength >= UMI_CLIPPED_EXPERIMENT_PATH_MAX ||
        pathLength > SIZE_MAX - suffixLength - 1U ||
        pathLength + suffixLength + 1U > capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outPath, checkpointPath, pathLength);
    memcpy(outPath + pathLength, suffix, suffixLength + 1U);
    return UMI_STATUS_OK;
}

static UmiStatus RunLockPath(const char *checkpointPath,
                             char *outPath, size_t capacity)
{
    const char suffix[] = ".training-lock";
    size_t pathLength, suffixLength = sizeof(suffix) - 1U;
    if (checkpointPath == NULL || checkpointPath[0] == '\0' ||
        outPath == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    pathLength = strlen(checkpointPath);
    if (pathLength >= UMI_CLIPPED_EXPERIMENT_PATH_MAX ||
        pathLength > SIZE_MAX - suffixLength - 1U ||
        pathLength + suffixLength + 1U > capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outPath, checkpointPath, pathLength);
    memcpy(outPath + pathLength, suffix, suffixLength + 1U);
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    UmiAiTrainingExperimentConfig plan;
    UmiAiTrainingExperimentAccumulation accumulation = {0};
    UmiAiTrainingAdamWConfig optimiserConfig;
    UmiAiTrainingLearningRateSchedule schedule;
    UmiAiTrainingExperimentResult report;
    UmiAiTrainingGradientClipReport lastClip = {0};
    UmiAiTrainingGradientPolicy policy;
    UmiAiTrainingRunLockInputs lockInputs;
    int strictRunLock = 0;
    int strictSourceLock = 0;
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL, *checkpointModel = NULL;
    UmiAiTrainingAdamW *optimizer = NULL, *checkpointOptimizer = NULL;
    uint8_t *checkpointBytes = NULL;
    size_t checkpointByteCount = 0U, microbatchSequences;
    char manifest[UMI_CLIPPED_EXPERIMENT_PATH_MAX];
    char sidecar[UMI_CLIPPED_EXPERIMENT_PATH_MAX + 32U];
    char runLockPath[UMI_CLIPPED_EXPERIMENT_PATH_MAX + 32U];
    char sourceLockPath[UMI_CLIPPED_EXPERIMENT_PATH_MAX + 32U];
    float maximumGradientNorm = 0.0F;
    uint64_t scheduleSteps = 0U;
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;

    if (argc >= 4 && strcmp(argv[argc - 1], "--strict-source-lock") == 0) {
        /* Cryptographic source identity REQUIRES the earlier strict run lock;
         * neither the previous flags nor default behaviour is weakened. */
        strictSourceLock = 1;
        strictRunLock = 1;
        --argc;
    } else if (argc >= 4 && strcmp(argv[argc - 1], "--strict-run-lock") == 0) {
        strictRunLock = 1;
        /* Only this opt-in trailing argument is consumed. Earlier invocation
         * shapes and the old .clip-policy-only contract remain unchanged. */
        --argc;
    }
    if (argc < 3 || argc > 5 || !ReadUnsigned(argv[2], &scheduleSteps) ||
        scheduleSteps > (uint64_t)UMI_AI_EXPERIMENT_MAX_STEPS) {
        fputs("usage: umicom-native-llm-adamw-clipped-experiment "
              "<reviewed-plan> <schedule-total-steps> "
              "[NEW-combined-checkpoint] [existing-combined-checkpoint] "
              "[--strict-run-lock | --strict-source-lock]\n",
              stderr);
        return 2;
    }
    status = UmiAiTrainingExperimentPlanLoadAdamW(
        argv[1], &plan, &accumulation, &maximumGradientNorm,
        manifest, sizeof(manifest));
    if (status != UMI_STATUS_OK) goto fail;
    if (!(maximumGradientNorm > 0.0F)) {
        fputs("reviewed plan must explicitly set maximum_gradient_norm\n",
              stderr);
        status = UMI_STATUS_INVALID_ARGUMENT;
        goto fail;
    }
    /* This initial clipped experiment uses the established bounded AdamW
     * experiment sampler. A supplied effective_sequences must match its
     * existing batch_sequences; never claim to execute a larger batch while
     * silently sampling fewer sequences. The decoder still accumulates
     * gradients across microbatches within that approved effective batch. */
    if (accumulation.effectiveSequences != 0U &&
        accumulation.effectiveSequences != plan.batchSequences) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        goto fail;
    }
    microbatchSequences = accumulation.microbatchSequences == 0U ?
        (size_t)plan.batchSequences :
        (size_t)accumulation.microbatchSequences;
    policy = (UmiAiTrainingGradientPolicy){
        maximumGradientNorm, plan.batchSequences, (uint32_t)microbatchSequences};
    status = UmiAiTrainingAdamWDefaults(&optimiserConfig);
    if (status != UMI_STATUS_OK) goto fail;
    schedule = (UmiAiTrainingLearningRateSchedule){
        UMI_AI_TRAINING_SCHEDULE_COSINE, plan.learningRate,
        plan.learningRate * 0.1F,
        scheduleSteps > UINT64_C(4) ? UINT64_C(2) : UINT64_C(0),
        scheduleSteps};
    status = UmiAiTrainingExperimentAdamWValidate(
        &plan, &optimiserConfig, &schedule);
    if (status != UMI_STATUS_OK) goto fail;

    /* Refuse an occupied destination before doing expensive model work.
     * No-clobber guarantees remain enforced by the checkpoint and policy
     * publication services; this early check is only a usability check. */
    if (argc >= 4) {
        FILE *existing;
        status = CompanionPath(argv[3], sidecar, sizeof(sidecar));
        if (status != UMI_STATUS_OK) goto fail;
        existing = fopen(argv[3], "rb");
        if (existing != NULL) {
            fclose(existing);
            status = UMI_STATUS_ALREADY_EXISTS;
            goto fail;
        }
        existing = fopen(sidecar, "rb");
        if (existing != NULL) {
            fclose(existing);
            status = UMI_STATUS_ALREADY_EXISTS;
            goto fail;
        }
        if (strictRunLock) {
            status = RunLockPath(argv[3], runLockPath, sizeof(runLockPath));
            if (status != UMI_STATUS_OK) goto fail;
            existing = fopen(runLockPath, "rb");
            if (existing != NULL) {
                fclose(existing);
                status = UMI_STATUS_ALREADY_EXISTS;
                goto fail;
            }
        }
        if (strictSourceLock) {
            status = SourceLockPath(argv[3], sourceLockPath,
                                    sizeof(sourceLockPath));
            if (status != UMI_STATUS_OK) goto fail;
            existing = fopen(sourceLockPath, "rb");
            if (existing != NULL) {
                fclose(existing);
                status = UMI_STATUS_ALREADY_EXISTS;
                goto fail;
            }
        }
    }
    if (argc == 5) {
        /* Verify provenance record before ANY optimisation, including a
         * model resume with zero outstanding steps. Merely matching a scalar
         * threshold is insufficient: the record binds the exact combined
         * checkpoint bytes and selected microbatch policy as well. */
        status = CompanionPath(argv[4], sidecar, sizeof(sidecar));
        if (status != UMI_STATUS_OK) goto fail;
        status = UmiAiTrainingAdamWCheckpointLoadFile(
            argv[4], &checkpointModel, &checkpointOptimizer);
        if (status != UMI_STATUS_OK) goto fail;
        status = EncodePair(checkpointModel, checkpointOptimizer,
                            &checkpointBytes, &checkpointByteCount);
        if (status != UMI_STATUS_OK) goto fail;
        status = UmiAiTrainingGradientPolicyVerifyFile(
            sidecar, checkpointBytes, checkpointByteCount, &policy);
        free(checkpointBytes);
        checkpointBytes = NULL;
        checkpointByteCount = 0U;
        if (status != UMI_STATUS_OK) {
            fputs("checkpoint clipping policy missing or mismatched; "
                  "resume refused\n", stderr);
            goto fail;
        }
    }
    status = UmiAiTrainingDatasetLoadManifest(manifest, &dataset);
    if (status != UMI_STATUS_OK) goto fail;
    lockInputs = (UmiAiTrainingRunLockInputs){
        dataset, &plan, &optimiserConfig, &schedule, &policy};
    if (strictRunLock && argc == 5) {
        /* Existing checkpoints from the historical clipped laboratory have
         * no .training-lock. Fail closed in strict mode: review and explicitly
         * start a new strict experiment rather than inventing provenance. */
        status = RunLockPath(argv[4], runLockPath, sizeof(runLockPath));
        if (status != UMI_STATUS_OK) goto fail;
        status = EncodePair(checkpointModel, checkpointOptimizer,
                            &checkpointBytes, &checkpointByteCount);
        if (status != UMI_STATUS_OK) goto fail;
        status = UmiAiTrainingRunLockVerifyFile(
            runLockPath, checkpointBytes, checkpointByteCount, &lockInputs);
        free(checkpointBytes);
        checkpointBytes = NULL;
        checkpointByteCount = 0U;
        if (status != UMI_STATUS_OK) {
            fputs("strict run-lock missing or mismatched; resume refused\n",
                  stderr);
            goto fail;
        }
    }
    if (strictSourceLock && argc == 5) {
        /* Hash source bytes from the immutable dataset snapshot, not from
         * an independent path re-open. No optimisation has begun yet. */
        status = SourceLockPath(argv[4], sourceLockPath,
                                sizeof(sourceLockPath));
        if (status != UMI_STATUS_OK) goto fail;
        status = EncodePair(checkpointModel, checkpointOptimizer,
                            &checkpointBytes, &checkpointByteCount);
        if (status != UMI_STATUS_OK) goto fail;
        status = UmiAiTrainingSourceLockVerifyFile(
            sourceLockPath, checkpointBytes, checkpointByteCount, dataset);
        free(checkpointBytes);
        checkpointBytes = NULL;
        checkpointByteCount = 0U;
        if (status != UMI_STATUS_OK) {
            fputs("cryptographic source-lock missing or mismatched; "
                  "resume refused\n", stderr);
            goto fail;
        }
    }
    if (argc == 5) {
        status = UmiAiTrainingExperimentResumeAdamWClipped(
            dataset, &plan, &optimiserConfig, &schedule,
            checkpointModel, checkpointOptimizer,
            microbatchSequences, maximumGradientNorm,
            Progress, NULL, &model, &optimizer, &report, &lastClip);
    } else {
        status = UmiAiTrainingExperimentRunAdamWClipped(
            dataset, &plan, &optimiserConfig, &schedule,
            microbatchSequences, maximumGradientNorm,
            Progress, NULL, &model, &optimizer, &report, &lastClip);
    }
    if (status != UMI_STATUS_OK) goto fail;
    printf("AdamW: %" PRIu64 " steps; held-out CE %.6f -> %.6f\n",
           report.completedSteps, report.baselineHeldOutLoss,
           report.finalHeldOutLoss);
    printf("last-step gradient norm %.9g | applied scale %.9g | clipped %s\n",
           lastClip.globalNorm, lastClip.appliedScale,
           lastClip.wasClipped ? "yes" : "no");
    puts("Checkpoints alone do not attest dataset identity, provenance "
         "or host trust. Review all sources and settings before continuing.");

    if (argc >= 4) {
        /* Publication is intentionally fail-closed: if creating the second
         * companion file fails, a checkpoint without policy remains unusable
         * by this command. A pair cannot be committed atomically across two
         * filesystem paths. The operator reviews any incomplete pair. */
        status = UmiAiTrainingAdamWCheckpointSaveNewFile(
            model, optimizer, argv[3]);
        if (status != UMI_STATUS_OK) goto fail;
        status = EncodePair(model, optimizer,
                            &checkpointBytes, &checkpointByteCount);
        if (status != UMI_STATUS_OK) goto fail;
        status = CompanionPath(argv[3], sidecar, sizeof(sidecar));
        if (status != UMI_STATUS_OK) goto fail;
        status = UmiAiTrainingGradientPolicySaveNewFile(
            sidecar, checkpointBytes, checkpointByteCount, &policy);
        if (status != UMI_STATUS_OK) goto fail;
        if (strictRunLock) {
            status = RunLockPath(argv[3], runLockPath, sizeof(runLockPath));
            if (status != UMI_STATUS_OK) goto fail;
            status = UmiAiTrainingRunLockSaveNewFile(
                runLockPath, checkpointBytes, checkpointByteCount, &lockInputs);
            if (status != UMI_STATUS_OK) goto fail;
        }
        if (strictSourceLock) {
            /* Incomplete bundles fail closed on the next resume. This is
             * deliberately an additive companion, not a checkpoint rewrite. */
            status = SourceLockPath(argv[3], sourceLockPath,
                                    sizeof(sourceLockPath));
            if (status != UMI_STATUS_OK) goto fail;
            status = UmiAiTrainingSourceLockSaveNewFile(
                sourceLockPath, checkpointBytes, checkpointByteCount, dataset);
            if (status != UMI_STATUS_OK) goto fail;
        }
        printf("New combined checkpoint and clipping policy: %s\n", argv[3]);
        if (strictRunLock)
            printf("Additional dataset/schedule/AdamW run lock: %s\n",
                   runLockPath);
        if (strictSourceLock)
            printf("Cryptographic immutable source-snapshot lock: %s\n",
                   sourceLockPath);
    }
    free(checkpointBytes);
    UmiAiTrainingAdamWDestroy(optimizer);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingAdamWDestroy(checkpointOptimizer);
    UmiAiTrainingDecoderDestroy(checkpointModel);
    UmiAiTrainingDatasetDestroy(dataset);
    return 0;
fail:
    fprintf(stderr, "reviewed clipped AdamW experiment rejected (status=%d)\n",
            (int)status);
    free(checkpointBytes);
    UmiAiTrainingAdamWDestroy(optimizer);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingAdamWDestroy(checkpointOptimizer);
    UmiAiTrainingDecoderDestroy(checkpointModel);
    UmiAiTrainingDatasetDestroy(dataset);
    return 1;
}
