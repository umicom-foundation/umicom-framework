/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/approval.h
 *
 * PURPOSE:
 *   Represent a human or policy approval decision for sensitive AI actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Approval is a first-class record so tool execution and remote calls can be governed and audited.
 */

#ifndef INCLUDE_UMICOM_AI_APPROVAL_H
#define INCLUDE_UMICOM_AI_APPROVAL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiApproval {
    char approval_id[UMI_AI_ID_CAPACITY];
    char action[UMI_AI_SMALL_TEXT_CAPACITY];
    int required;
    int approved;
    char approved_by[UMI_AI_ID_CAPACITY];
} UmiAiApproval;

UmiStatus umi_ai_approval_init(UmiAiApproval *value);

#ifdef __cplusplus
}
#endif

#endif
