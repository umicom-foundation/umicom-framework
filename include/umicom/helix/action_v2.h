/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/action_v2.h
 * PURPOSE: Describe one bounded, inspectable Helix v2 runtime action.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: An action is data first.  An adapter may execute it only
 * after the approval gate and policy have accepted the exact descriptor. */
#ifndef INCLUDE_UMICOM_HELIX_ACTION_V2_H
#define INCLUDE_UMICOM_HELIX_ACTION_V2_H

#include "umicom/base/status.h"
#include "umicom/helix/types.h"
#include "umicom/helix/v2_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixActionV2 {
    char action_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixActionKindV2 kind;
    char summary[UMI_HELIX_TEXT_CAPACITY];
    char target[UMI_HELIX_TEXT_CAPACITY];
    uint32_t attempt;
    uint32_t maximum_attempts;
    int mutating;
} UmiHelixActionV2;

UmiStatus umi_helix_action_v2_init(UmiHelixActionV2 *action,
                                   const char *action_id,
                                   UmiHelixActionKindV2 kind,
                                   const char *summary,
                                   const char *target,
                                   uint32_t maximum_attempts);
int umi_helix_action_v2_requires_approval(const UmiHelixActionV2 *action);

#ifdef __cplusplus
}
#endif

#endif
