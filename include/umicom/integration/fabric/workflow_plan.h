/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/workflow_plan.h
 *
 * PURPOSE:
 *   Topologically order workflow steps so dependencies are always executed first.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_WORKFLOW_PLAN_H
#define UMICOM_INTEGRATION_FABRIC_WORKFLOW_PLAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/workflow_graph.h"
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the fabric workflow plan data shared with callers of this public contract.
 */
typedef struct UmiFabricWorkflowPlan { char step_ids[UMI_FABRIC_MAX_STEPS][UMI_FABRIC_ID_CAPACITY]; size_t count; uint64_t fingerprint; } UmiFabricWorkflowPlan;
/**
 * Provide the fabric workflow plan build operation used by this module and its client
 * applications.
 */
/* Validate the entire graph before ordering all steps. A dependency always
 * precedes its dependent; independent ready steps use lexical ID order.
 * Empty/cyclic input is INVALID_ARGUMENT; malformed storage is INVALID_STATE.
 * Temporary allocation may fail with OUT_OF_MEMORY. Every error leaves the
 * previous output unchanged. Callers serialize access and keep input/output
 * objects separate. Planning performs no model, file, network or tool action.
 * The retained fingerprint hashes ordered step IDs only, not the full graph,
 * operation settings or approval state; do not use it as execution authority. */
UmiStatus umi_fabric_workflow_plan_build(const UmiFabricWorkflowGraph *graph,UmiFabricWorkflowPlan *out_plan);

#ifdef __cplusplus
}
#endif
#endif
