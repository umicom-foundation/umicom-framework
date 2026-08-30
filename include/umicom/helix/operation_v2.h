/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/operation_v2.h
 * PURPOSE: Hold deterministic state for one governed Helix v2 operation.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: The state machine is deliberately strict so logs, UI and
 * recovery code all agree about what may happen next. */
#ifndef INCLUDE_UMICOM_HELIX_OPERATION_V2_H
#define INCLUDE_UMICOM_HELIX_OPERATION_V2_H

#include "umicom/base/status.h"
#include "umicom/helix/types.h"
#include "umicom/helix/v2_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixOperationV2 {
    char operation_id[UMI_HELIX_ID_CAPACITY];
    char objective[UMI_HELIX_TEXT_CAPACITY];
    char plan_hash[UMI_HELIX_V2_HASH_CAPACITY];
    UmiHelixOperationStateV2 state;
    uint64_t revision;
    uint32_t actions_completed;
    uint32_t evidence_count;
    uint32_t retry_count;
} UmiHelixOperationV2;

UmiStatus umi_helix_operation_v2_init(UmiHelixOperationV2 *operation,
                                      const char *operation_id,
                                      const char *objective,
                                      const char *plan_hash);
UmiStatus umi_helix_operation_v2_transition(
    UmiHelixOperationV2 *operation,
    UmiHelixOperationStateV2 next);

#ifdef __cplusplus
}
#endif

#endif
