/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/plan.h
 *
 * PURPOSE:
 *   Describe one bounded change plan and its intended validation gates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Plans make intended scope visible before an agent edits files, reducing uncontrolled changes outside the approved boundary.
 */

#ifndef INCLUDE_UMICOM_HELIX_PLAN_H
#define INCLUDE_UMICOM_HELIX_PLAN_H

#include <stdint.h>
#include "umicom/helix/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixPlan {
    char plan_id[UMI_HELIX_ID_CAPACITY];
    char objective[UMI_HELIX_TEXT_CAPACITY];
    char scope[UMI_HELIX_TEXT_CAPACITY];
    uint32_t required_gates;
} UmiHelixPlan;

UmiStatus umi_helix_plan_init(UmiHelixPlan *value);

#ifdef __cplusplus
}
#endif

#endif
