/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/layout_snapshot.h
 *
 * PURPOSE:
 *   normalised layout snapshot state and deterministic layout fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_LAYOUT_SNAPSHOT_H
#define UMICOM_FRONTEND_CONFORMANCE_LAYOUT_SNAPSHOT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc layout snapshot data shared with callers of this public contract.
 */
typedef struct UmiFcLayoutSnapshot { char id[UMI_FC_ID_CAPACITY]; double values[UMI_FC_MAX_LAYOUT_VALUES]; size_t count; uint64_t revision; uint64_t fingerprint; } UmiFcLayoutSnapshot;
/**
 * Initialise fc layout snapshot from caller-provided values so later operations receive a
 * known state.
 */
void umi_fc_layout_snapshot_init(UmiFcLayoutSnapshot *snapshot,const char *id);
/**
 * Add fc layout snapshot only after its inputs and available capacity have been checked.
 */
UmiStatus umi_fc_layout_snapshot_append(UmiFcLayoutSnapshot *snapshot,double value);
/**
 * Provide the fc layout snapshot fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_layout_snapshot_fingerprint(UmiFcLayoutSnapshot *snapshot);

#ifdef __cplusplus
}
#endif
#endif
