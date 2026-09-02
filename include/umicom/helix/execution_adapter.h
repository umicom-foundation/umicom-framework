/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/execution_adapter.h
 * PURPOSE: Define controlled host callbacks for executing Helix v2 actions.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Framework never edits a product workspace directly.  Studio
 * supplies these callbacks and the gate checks approval before dispatch. */
#ifndef INCLUDE_UMICOM_HELIX_EXECUTION_ADAPTER_H
#define INCLUDE_UMICOM_HELIX_EXECUTION_ADAPTER_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/helix/action.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiHelixExecuteAction)(
    void *context,
    const UmiHelixAction *action,
    char *out_evidence,
    size_t evidence_capacity);

/**
 * Represent the helix execution adapter data shared with callers of this public contract.
 */
typedef struct UmiHelixExecutionAdapter {
    void *context;
    UmiHelixExecuteAction read;
    UmiHelixExecuteAction filesystem;
    UmiHelixExecuteAction build;
    UmiHelixExecuteAction test;
    UmiHelixExecuteAction review;
    UmiHelixExecuteAction source_control;
} UmiHelixExecutionAdapter;

/**
 * Check that helix execution adapter satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_helix_execution_adapter_validate(
    const UmiHelixExecutionAdapter *adapter);
/**
 * Perform helix execution adapter through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_helix_execution_adapter_execute(
    const UmiHelixExecutionAdapter *adapter,
    const UmiHelixAction *action,
    int approved,
    char *out_evidence,
    size_t evidence_capacity);

#ifdef __cplusplus
}
#endif

#endif
