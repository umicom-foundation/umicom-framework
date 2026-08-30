/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/action.h
 * PURPOSE: Describe one bounded, inspectable Helix v2 runtime action.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: An action is data first.  An adapter may execute it only
 * after the approval gate and policy have accepted the exact descriptor. */
#ifndef INCLUDE_UMICOM_HELIX_ACTION_H
#define INCLUDE_UMICOM_HELIX_ACTION_H

#include "umicom/base/status.h"
#include "umicom/helix/types.h"
#include "umicom/helix/execution_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixAction {
    char action_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixActionKind kind;
    char summary[UMI_HELIX_TEXT_CAPACITY];
    char target[UMI_HELIX_TEXT_CAPACITY];
    uint32_t attempt;
    uint32_t maximum_attempts;
    int mutating;
} UmiHelixAction;

UmiStatus umi_helix_action_init(UmiHelixAction *action,
                                   const char *action_id,
                                   UmiHelixActionKind kind,
                                   const char *summary,
                                   const char *target,
                                   uint32_t maximum_attempts);
int umi_helix_action_requires_approval(const UmiHelixAction *action);

#ifdef __cplusplus
}
#endif

#endif
