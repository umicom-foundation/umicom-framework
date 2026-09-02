/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/approval.h
 *
 * PURPOSE:
 *   Record a human or policy approval decision for a Helix candidate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Approval remains separate from candidate state so audit records preserve who authorised a promotion and why.
 */

#ifndef INCLUDE_UMICOM_HELIX_APPROVAL_H
#define INCLUDE_UMICOM_HELIX_APPROVAL_H

#include <stdint.h>
#include "umicom/helix/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the helix approval data shared with callers of this public contract.
 */
typedef struct UmiHelixApproval {
    char candidate_id[UMI_HELIX_ID_CAPACITY];
    char approver[UMI_HELIX_ID_CAPACITY];
    int approved;
    char reason[UMI_HELIX_TEXT_CAPACITY];
} UmiHelixApproval;

/**
 * Initialise helix approval from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_helix_approval_init(UmiHelixApproval *value);

#ifdef __cplusplus
}
#endif

#endif
