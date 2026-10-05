/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/workflow_step.h
 *
 * PURPOSE:
 *   Describe an orchestrated integration step with timeout and compensation metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_WORKFLOW_STEP_H
#define UMICOM_INTEGRATION_FABRIC_WORKFLOW_STEP_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric workflow step data shared with callers of this public contract.
 */
typedef struct UmiFabricWorkflowStep {
    char step_id[UMI_FABRIC_ID_CAPACITY];
    char operation_id[UMI_FABRIC_ID_CAPACITY];
    uint64_t timeout_ms;
    bool optional;
    bool compensatable;
} UmiFabricWorkflowStep;

/**
 * Initialise fabric workflow step from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_fabric_workflow_step_init(UmiFabricWorkflowStep *item, const char *step_id, const char *operation_id, uint64_t timeout_ms, bool optional, bool compensatable);
/**
 * Check that fabric workflow step satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_workflow_step_validate(const UmiFabricWorkflowStep *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fabric_workflow_step_archive_encode(const UmiFabricWorkflowStep *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fabric_workflow_step_archive_decode(const void *bytes, size_t byte_count,
    UmiFabricWorkflowStep *value);

#ifdef __cplusplus
}
#endif
#endif
