/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/evaluation.c
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

#include "umicom/ai/evaluation.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise ai evaluation from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ai_evaluation_init(UmiAiEvaluation *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
