/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/snapshot_compare.h
 *
 * PURPOSE:
 *   semantic render snapshot comparison with tolerant volatile-field handling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_SNAPSHOT_COMPARE_H
#define UMICOM_FRONTEND_CONFORMANCE_SNAPSHOT_COMPARE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"
#include "umicom/frontend/conformance/render_snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc snapshot comparison data shared with callers of this public contract.
 */
typedef struct UmiFcSnapshotComparison { double node_score; double text_score; double interaction_score; bool fingerprint_match; double total_score; } UmiFcSnapshotComparison;
/**
 * Perform fc snapshot compare through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_fc_snapshot_compare_run(const UmiFcRenderSnapshot *expected,const UmiFcRenderSnapshot *actual,UmiFcSnapshotComparison *out_comparison);

#ifdef __cplusplus
}
#endif
#endif
