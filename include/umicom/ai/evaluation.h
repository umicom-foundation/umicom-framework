/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/evaluation.h
 *
 * PURPOSE:
 *   Record one AI evaluation score and pass/fail decision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Evaluation evidence is data, allowing Helix and Studio to compare candidate outputs without trusting prose descriptions.
 */

#ifndef INCLUDE_UMICOM_AI_EVALUATION_H
#define INCLUDE_UMICOM_AI_EVALUATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiEvaluation {
    char evaluation_id[UMI_AI_ID_CAPACITY];
    char metric[UMI_AI_ID_CAPACITY];
    double score;
    double threshold;
    int passed;
} UmiAiEvaluation;

UmiStatus umi_ai_evaluation_init(UmiAiEvaluation *value);

#ifdef __cplusplus
}
#endif

#endif
