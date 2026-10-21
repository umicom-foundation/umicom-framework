/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/experiment_plan.h
 *
 * PURPOSE:
 *   Strict, bounded, native-C23 training experiment plan input.
 *
 * SECURITY AND LIFETIME:
 *   Experiment plans are explicit, reviewed local text, not executable
 *   scripts. The manifest path is relative to the plan file, NOT the current
 *   working directory. Path grammar rejects absolute paths and dot traversal
 *   but is not a filesystem sandbox (symlinks can escape). The caller must
 *   approve datasets before running training. The Framework does not mutate
 *   files during plan parsing and does not retain caller buffers.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_EXPERIMENT_PLAN_H
#define UMICOM_AI_TRAINING_EXPERIMENT_PLAN_H

#include <stddef.h>
#include "umicom/ai/training/experiment.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_EXPERIMENT_MAX_PLAN_BYTES 4096U
#define UMI_AI_EXPERIMENT_MAX_MANIFEST_RELATIVE 160U

/* Accepted plain ASCII key=value format (LF or CRLF):
 *   UMICOM-AI-EXPERIMENT-1
 *   manifest=manifest.txt
 *   steps=32
 *   eval_interval=8
 *   eval_batches=4
 *   batch_sequences=1
 *   learning_rate=0.025
 *   model_seed=20261021
 *   train_seed=4477
 *   eval_seed=5544
 *   context=8
 *   channels=8
 *   hidden_channels=16
 *   attention_heads=2
 *
 * Only manifest is mandatory; missing optional settings inherit published
 * defaults from experiment.h. Every unknown/duplicate key or malformed value
 * fails. Comments must begin with '#' in column one. No expansion, quoting,
 * environment interpolation, includes or shell commands are supported.
 * Rejected calls preserve all output buffers. */
UmiStatus UmiAiTrainingExperimentPlanParse(
    const char *text, size_t textLength,
    UmiAiTrainingExperimentConfig *outConfig,
    char *outRelativeManifest, size_t manifestCapacity);

/* Read a complete bounded file, then resolve its relative manifest against
 * the plan's directory. On failure all outputs remain unchanged. */
UmiStatus UmiAiTrainingExperimentPlanLoadFile(
    const char *planFile,
    UmiAiTrainingExperimentConfig *outConfig,
    char *outManifestPath, size_t manifestCapacity);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_EXPERIMENT_PLAN_H */
