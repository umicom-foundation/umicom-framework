/*-----------------------------------------------------------------------------
 * Umicom Framework | test_training_experiment_plan.c
 * Author: Sammy Hegab, Umicom Foundation | MIT
 * PURPOSE: Strict, reproducible native experiment-plan parsing.
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "CHECK failed at %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

static int RejectWithoutMutation(const char *source)
{
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingExperimentConfig preserved;
    char manifest[128] = "KEEP ME";
    UmiStatus status;
    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    memcpy(&preserved, &config, sizeof(config));
    status = UmiAiTrainingExperimentPlanParse(source, strlen(source),
                                               &config, manifest,
                                               sizeof(manifest));
    CHECK(status != UMI_STATUS_OK);
    CHECK(strcmp(manifest, "KEEP ME") == 0);
    CHECK(memcmp(&config, &preserved, sizeof(config)) == 0);
    return 0;
}

int main(void)
{
    const char plan[] =
        "# reviewed plan\r\nUMICOM-AI-EXPERIMENT-1\r\n"
        "manifest=manifest.txt\r\nsteps=4\r\n"
        "eval_interval=2\r\neval_batches=3\r\n"
        "learning_rate=0.125\r\ntrain_seed=0\r\n";
    UmiAiTrainingExperimentConfig candidate = {0};
    char path[128] = {0};
    CHECK(UmiAiTrainingExperimentPlanParse(plan, strlen(plan),
                                            &candidate, path,
                                            sizeof(path)) == UMI_STATUS_OK);
    CHECK(strcmp(path, "manifest.txt") == 0);
    CHECK(candidate.steps == 4U);
    CHECK(candidate.evalInterval == 2U);
    CHECK(candidate.evalBatches == 3U);
    CHECK(candidate.decoder.contextLength == 8U);
    CHECK(candidate.trainSeed == 0U);
    CHECK(candidate.learningRate == 0.125F);
    CHECK(RejectWithoutMutation("manifest=data.txt\n") == 0); /* no header */
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\n") == 0); /* no manifest */
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=../data.txt\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=/data.txt\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=C:\\secret.txt\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=./data.txt\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nmanifest=other.txt\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nunknown=1\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nsteps=-1\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nsteps=9999999999999999999999\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nlearning_rate=nan\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nlearning_rate=0.0\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nlearning_rate=0.1e0\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\ncontext=30\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nsteps=0\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nattention_heads=3\n") == 0);
    CHECK(RejectWithoutMutation("UMICOM-AI-EXPERIMENT-1\nmanifest=ok.txt\nsteps=5\neval_interval=6\n") == 0);
    /* A rejected too-small output has no partial copied bytes or config. */
    {
        char tooSmall[2] = {'X', '\0'};
        UmiAiTrainingExperimentConfig saved;
        memcpy(&saved, &candidate, sizeof(saved));
        CHECK(UmiAiTrainingExperimentPlanParse(plan, strlen(plan),
                &candidate, tooSmall, sizeof(tooSmall)) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(tooSmall[0] == 'X');
        CHECK(memcmp(&candidate, &saved, sizeof(candidate)) == 0);
    }
    puts("native experiment plans: PASS");
    return 0;
}
