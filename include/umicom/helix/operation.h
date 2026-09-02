/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/operation.h
 * PURPOSE: Hold deterministic state for one governed Helix v2 operation.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* The state machine is deliberately strict so logs, UI and
 * recovery code all agree about what may happen next. */
#ifndef INCLUDE_UMICOM_HELIX_OPERATION_H
#define INCLUDE_UMICOM_HELIX_OPERATION_H

#include "umicom/base/status.h"
#include "umicom/helix/types.h"
#include "umicom/helix/execution_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the helix operation data shared with callers of this public contract.
 */
typedef struct UmiHelixOperation {
    char operation_id[UMI_HELIX_ID_CAPACITY];
    char objective[UMI_HELIX_TEXT_CAPACITY];
    char plan_hash[UMI_HELIX_RUNTIME_HASH_CAPACITY];
    UmiHelixOperationState state;
    uint64_t revision;
    uint32_t actions_completed;
    uint32_t evidence_count;
    uint32_t retry_count;
} UmiHelixOperation;

/**
 * Initialise helix operation from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_helix_operation_init(UmiHelixOperation *operation,
                                      const char *operation_id,
                                      const char *objective,
                                      const char *plan_hash);
/**
 * Provide the helix operation transition operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_operation_transition(
    UmiHelixOperation *operation,
    UmiHelixOperationState next);

#ifdef __cplusplus
}
#endif

#endif
