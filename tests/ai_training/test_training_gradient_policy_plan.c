/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_gradient_policy_plan.c
 * PURPOSE: Verify reviewed, opt-in global-gradient clipping plan semantics,
 *          exact numeric parsing and rejection without output mutations.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * LICENCE: MIT
 */
#include "umicom/ai/training/experiment_plan.h"
#include <stdio.h>
#include <string.h>

#ifndef UMI_AI_TEST_CLIPPED_PLAN_PATH
#error UMI_AI_TEST_CLIPPED_PLAN_PATH must be provided by CMake
#endif
#ifndef UMI_AI_TEST_CLIPPED_MANIFEST_PATH
#error UMI_AI_TEST_CLIPPED_MANIFEST_PATH must be provided by CMake
#endif

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

static int RejectWithoutMutation(const char *source)
{
    UmiAiTrainingExperimentConfig config, saved;
    UmiAiTrainingExperimentAccumulation batching = {91U, 82U}, savedBatching;
    float maximum = 73.25F;
    char manifest[64] = "NOT MODIFIED";
    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    saved = config;
    savedBatching = batching;
    CHECK(UmiAiTrainingExperimentPlanParseAdamW(
        source, strlen(source), &config, &batching, &maximum,
        manifest, sizeof(manifest)) != UMI_STATUS_OK);
    CHECK(memcmp(&config, &saved, sizeof(saved)) == 0);
    CHECK(memcmp(&batching, &savedBatching, sizeof(batching)) == 0);
    CHECK(maximum == 73.25F);
    CHECK(strcmp(manifest, "NOT MODIFIED") == 0);
    return 0;
}

int main(void)
{
    const char valid[] = "UMICOM-AI-EXPERIMENT-1\nmanifest=manifest.txt\n"
                         "steps=4\neval_interval=2\n"
                         "batch_sequences=2\n"
                         "effective_sequences=2\nmicrobatch_sequences=1\n"
                         "maximum_gradient_norm=1.25\n";
    const char disabled[] = "UMICOM-AI-EXPERIMENT-1\nmanifest=manifest.txt\n";
    UmiAiTrainingExperimentConfig config = {0};
    UmiAiTrainingExperimentAccumulation batching = {0};
    float norm = -4.0F;
    char manifest[128] = {0};
    CHECK(UmiAiTrainingExperimentPlanParseAdamW(
        valid, strlen(valid), &config, &batching, &norm,
        manifest, sizeof(manifest)) == UMI_STATUS_OK);
    CHECK(strcmp(manifest, "manifest.txt") == 0);
    CHECK(norm == 1.25F && config.batchSequences == 2U);
    CHECK(batching.effectiveSequences == 2U && batching.microbatchSequences == 1U);
    CHECK(UmiAiTrainingExperimentPlanParseAdamW(
        disabled, strlen(disabled), &config, &batching, &norm,
        manifest, sizeof(manifest)) == UMI_STATUS_OK);
    CHECK(norm == 0.0F && batching.effectiveSequences == 0U);
    /* The file loader resolves the manifest relative to its approved plan,
     * not relative to the process's working directory. Every output remains
     * valid after the function returns: no caller-borrowed temporary paths. */
    CHECK(UmiAiTrainingExperimentPlanLoadAdamW(
        UMI_AI_TEST_CLIPPED_PLAN_PATH, &config, &batching, &norm,
        manifest, sizeof(manifest)) == UMI_STATUS_OK);
    CHECK(strcmp(manifest, UMI_AI_TEST_CLIPPED_MANIFEST_PATH) == 0);
    CHECK(norm == 1.0F && config.steps == 4U &&
          batching.effectiveSequences == 2U &&
          batching.microbatchSequences == 1U);
    CHECK(UmiAiTrainingExperimentPlanLoadFile(
        UMI_AI_TEST_CLIPPED_PLAN_PATH, &config,
        manifest, sizeof(manifest)) == UMI_STATUS_PARSE_ERROR);
    /* The established strict parser must not accept an unrecognised policy. */
    CHECK(UmiAiTrainingExperimentPlanParse(
        valid, strlen(valid), &config, manifest,
        sizeof(manifest)) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiAiTrainingExperimentPlanParseAccumulated(
        valid, strlen(valid), &config, &batching, manifest,
        sizeof(manifest)) == UMI_STATUS_PARSE_ERROR);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nmaximum_gradient_norm=0\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nmaximum_gradient_norm=-1\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nmaximum_gradient_norm=nan\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nmaximum_gradient_norm=inf\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nmaximum_gradient_norm=1e3\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nmaximum_gradient_norm=1,5\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nmaximum_gradient_norm=1.\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nmaximum_gradient_norm=0.0000000001\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nmaximum_gradient_norm=1234567890\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nmaximum_gradient_norm=1\nmaximum_gradient_norm=1\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nmaximum_gradient_norm=1\neffective_sequences=2\n") == 0);
    /* Too-small caller storage never receives a partial manifest or norm. */
    {
        float preservedNorm = 7.0F;
        char tooSmall[] = "X";
        CHECK(UmiAiTrainingExperimentPlanParseAdamW(
            valid, strlen(valid), &config, &batching, &preservedNorm,
            tooSmall, sizeof(tooSmall)) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(preservedNorm == 7.0F && tooSmall[0] == 'X');
    }
    puts("AdamW clipping plan grammar and failure atomicity: PASS");
    return 0;
}
