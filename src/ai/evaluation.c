/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/evaluation.c
 *
 * PURPOSE:
 *   Record one AI evaluation score and pass/fail decision.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Evaluation evidence is data, allowing Helix and Studio to compare candidate outputs without trusting prose descriptions.
 */

#include "umicom/ai/evaluation.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_ai_evaluation_init(UmiAiEvaluation *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
