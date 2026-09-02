/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/plan.h
 *
 * PURPOSE:
 *   Describe one bounded change plan and its intended validation gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the helix plan data shared with callers of this public contract.
 */
typedef struct UmiHelixPlan {
    char plan_id[UMI_HELIX_ID_CAPACITY];
    char objective[UMI_HELIX_TEXT_CAPACITY];
    char scope[UMI_HELIX_TEXT_CAPACITY];
    uint32_t required_gates;
} UmiHelixPlan;

/**
 * Initialise helix plan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_helix_plan_init(UmiHelixPlan *value);

#ifdef __cplusplus
}
#endif

#endif
