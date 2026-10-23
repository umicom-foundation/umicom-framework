/*-----------------------------------------------------------------------------
 * Umicom Framework | learning/native-llm/compare_model_quality.c
 *
 * PURPOSE:
 *   Compare two independently reviewed native AdamW checkpoints on the same
 *   fixed-seed held-out source windows, using the Framework's real evaluator.
 *
 * SECURITY:
 *   Only trusted local checkpoint and plan paths may be supplied. This tool
 *   does not authenticate files or decide release authorisation. Use the
 *   separate signed-publication/release-approval verifier first. It does not
 *   write checkpoints, execute generated code or make network requests.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment_plan.h"
#include "umicom/ai/training/model_quality_gate.h"
#include "umicom/ai/training/optimizer_checkpoint.h"

#include <stdio.h>

#define UMI_MODEL_QUALITY_MANIFEST_CAPACITY 1024U

int main(int argc, char **argv)
{
    UmiAiTrainingExperimentConfig plan;
    UmiAiTrainingExperimentAccumulation accumulation;
    UmiAiTrainingModelQualityPolicy policy;
    UmiAiTrainingModelQualityReport report;
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *baseline = NULL, *candidate = NULL;
    UmiAiTrainingAdamW *baselineOptimizer = NULL, *candidateOptimizer = NULL;
    float maximumGradientNorm = 0.0F;
    char manifest[UMI_MODEL_QUALITY_MANIFEST_CAPACITY];
    UmiStatus status;
    int exitCode = 2;

    if (argc != 4) {
        fputs("usage: umicom-native-llm-quality-compare "
              "<reviewed-evaluation-plan> <trusted-baseline-adamw.umi> "
              "<trusted-candidate-adamw.umi>\n", stderr);
        return 2;
    }
    /* The opt-in AdamW parser accepts plans with or without clipping and
     * microbatch keys. The quality comparison uses ONLY common evaluation
     * settings; no optimiser update occurs in this executable. */
    status = UmiAiTrainingExperimentPlanLoadAdamW(
        argv[1], &plan, &accumulation, &maximumGradientNorm,
        manifest, sizeof(manifest));
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDatasetLoadManifest(manifest, &dataset);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingAdamWCheckpointLoadFile(
        argv[2], &baseline, &baselineOptimizer);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingAdamWCheckpointLoadFile(
        argv[3], &candidate, &candidateOptimizer);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingModelQualityDefaults(&policy);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingModelQualityEvaluate(
        baseline, candidate, dataset, &plan, &policy, &report);
    if (status != UMI_STATUS_OK) goto failed;

    printf("Native LLM model comparison — fixed-seed held-out windows\n"
           "Evaluated target tokens: %zu\n"
           "Baseline cross-entropy: %.7f; candidate: %.7f\n"
           "Measured cross-entropy reduction: %.7f\n"
           "Baseline perplexity: %.7f; candidate: %.7f\n"
           "Top-1 accuracy change: %.4f percentage points\n"
           "Top-5 accuracy change: %.4f percentage points\n"
           "Educational quality gate: %s\n",
           report.baseline.evaluatedTokens,
           report.baseline.heldOutCrossEntropy,
           report.candidate.heldOutCrossEntropy,
           report.crossEntropyReduction,
           report.baseline.heldOutPerplexity,
           report.candidate.heldOutPerplexity,
           report.topOneAccuracyChange * 100.0,
           report.topFiveAccuracyChange * 100.0,
           report.accepted != 0 ? "PASS" : "NOT PASSED");
    puts("This is a small sampled next-token comparison, not proof of coding "
         "competence, signed provenance or release approval.");
    exitCode = report.accepted != 0 ? 0 : 1;
    goto cleanup;

failed:
    fprintf(stderr, "native model quality comparison failed (status=%d)\n",
            (int)status);
cleanup:
    /* A loaded AdamW moment state borrows the associated decoder.
     * Destroy optimiser handles before their borrowed model handles. */
    UmiAiTrainingAdamWDestroy(candidateOptimizer);
    UmiAiTrainingAdamWDestroy(baselineOptimizer);
    UmiAiTrainingDecoderDestroy(candidate);
    UmiAiTrainingDecoderDestroy(baseline);
    UmiAiTrainingDatasetDestroy(dataset);
    return exitCode;
}
