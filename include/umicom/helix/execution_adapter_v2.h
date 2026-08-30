/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/execution_adapter_v2.h
 * PURPOSE: Define controlled host callbacks for executing Helix v2 actions.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Framework never edits a product workspace directly.  Studio
 * supplies these callbacks and the gate checks approval before dispatch. */
#ifndef INCLUDE_UMICOM_HELIX_EXECUTION_ADAPTER_V2_H
#define INCLUDE_UMICOM_HELIX_EXECUTION_ADAPTER_V2_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/helix/action_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiHelixExecuteActionV2)(
    void *context,
    const UmiHelixActionV2 *action,
    char *out_evidence,
    size_t evidence_capacity);

typedef struct UmiHelixExecutionAdapterV2 {
    void *context;
    UmiHelixExecuteActionV2 read;
    UmiHelixExecuteActionV2 filesystem;
    UmiHelixExecuteActionV2 build;
    UmiHelixExecuteActionV2 test;
    UmiHelixExecuteActionV2 review;
    UmiHelixExecuteActionV2 source_control;
} UmiHelixExecutionAdapterV2;

UmiStatus umi_helix_execution_adapter_v2_validate(
    const UmiHelixExecutionAdapterV2 *adapter);
UmiStatus umi_helix_execution_adapter_v2_execute(
    const UmiHelixExecutionAdapterV2 *adapter,
    const UmiHelixActionV2 *action,
    int approved,
    char *out_evidence,
    size_t evidence_capacity);

#ifdef __cplusplus
}
#endif

#endif
