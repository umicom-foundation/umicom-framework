/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_accumulation_plan.c
 * PURPOSE: Qualify strict, opt-in effective-batch plan fields and preserve
 *          legacy parser behaviour and failure-atomic public output buffers.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * LICENCE: MIT
 */
#include "umicom/ai/training/experiment_plan.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef UMI_AI_TEST_ACCUMULATED_PLAN_PATH
#error UMI_AI_TEST_ACCUMULATED_PLAN_PATH must be supplied by CMake
#endif
#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH must be supplied by CMake
#endif
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

static int RejectWithoutMutation(const char *source)
{
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingExperimentConfig preserved;
    UmiAiTrainingExperimentAccumulation options = {731U, 829U};
    UmiAiTrainingExperimentAccumulation preservedOptions;
    char manifest[128] = "UNCHANGED";
    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    preserved = config;
    preservedOptions = options;
    CHECK(UmiAiTrainingExperimentPlanParseAccumulated(
        source, strlen(source), &config, &options,
        manifest, sizeof(manifest)) != UMI_STATUS_OK);
    CHECK(memcmp(&config, &preserved, sizeof(config)) == 0);
    CHECK(memcmp(&options, &preservedOptions, sizeof(options)) == 0);
    CHECK(strcmp(manifest, "UNCHANGED") == 0);
    return 0;
}

int main(void)
{
    const char complete[] =
        "UMICOM-AI-EXPERIMENT-1\nmanifest=manifest.txt\n"
        "steps=4\neval_interval=2\nbatch_sequences=1\n"
        "effective_sequences=8\nmicrobatch_sequences=2\n";
    const char standard[] =
        "UMICOM-AI-EXPERIMENT-1\nmanifest=manifest.txt\nsteps=4\neval_interval=2\n";
    UmiAiTrainingExperimentConfig config = {0};
    UmiAiTrainingExperimentAccumulation options = {0};
    char relative[128] = {0};
    char manifest[1024] = {0};
    CHECK(UmiAiTrainingExperimentPlanParseAccumulated(
        complete, strlen(complete), &config, &options,
        relative, sizeof(relative)) == UMI_STATUS_OK);
    CHECK(strcmp(relative, "manifest.txt") == 0);
    CHECK(config.batchSequences == 1U && config.steps == 4U);
    CHECK(options.effectiveSequences == 8U &&
          options.microbatchSequences == 2U);
    CHECK(UmiAiTrainingExperimentPlanLoadAccumulated(
        UMI_AI_TEST_ACCUMULATED_PLAN_PATH, &config,
        &options, manifest, sizeof(manifest)) == UMI_STATUS_OK);
    CHECK(strcmp(manifest, UMI_AI_TEST_MANIFEST_PATH) == 0);
    CHECK(options.effectiveSequences == 8U &&
          options.microbatchSequences == 2U);
    /* The existing public parser still rejects unexpected keys, rather than
     * silently claiming to understand an accumulated experiment. */
    CHECK(UmiAiTrainingExperimentPlanParse(
        complete, strlen(complete), &config, relative,
        sizeof(relative)) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiAiTrainingExperimentPlanParseAccumulated(
        standard, strlen(standard), &config, &options,
        relative, sizeof(relative)) == UMI_STATUS_OK);
    CHECK(options.effectiveSequences == 0U &&
          options.microbatchSequences == 0U);
    CHECK(RejectWithoutMutation(
        "UMICOM-AI-EXPERIMENT-1\nmanifest=manifest.txt\n"
        "effective_sequences=8\n") == 0);
    CHECK(RejectWithoutMutation(
        "UMICOM-AI-EXPERIMENT-1\nmanifest=manifest.txt\n"
        "microbatch_sequences=2\n") == 0);
    CHECK(RejectWithoutMutation(
        "UMICOM-AI-EXPERIMENT-1\nmanifest=manifest.txt\n"
        "effective_sequences=8\neffective_sequences=8\n"
        "microbatch_sequences=2\n") == 0);
    CHECK(RejectWithoutMutation(
        "UMICOM-AI-EXPERIMENT-1\nmanifest=manifest.txt\n"
        "effective_sequences=-8\nmicrobatch_sequences=2\n") == 0);
    CHECK(RejectWithoutMutation(
        "UMICOM-AI-EXPERIMENT-1\nmanifest=manifest.txt\n"
        "effective_sequences=8\nmicrobatch_sequences=8\n") == 0);
    CHECK(RejectWithoutMutation(
        "UMICOM-AI-EXPERIMENT-1\nmanifest=manifest.txt\n"
        "effective_sequences=2\nmicrobatch_sequences=4\n") == 0);
    puts("Accumulation plan, strict legacy grammar and atomic parse: PASS");
    return 0;
}
