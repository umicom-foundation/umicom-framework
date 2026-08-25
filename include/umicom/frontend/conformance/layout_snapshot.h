/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/layout_snapshot.h
 *
 * PURPOSE:
 *   normalised layout snapshot state and deterministic layout fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFcLayoutSnapshot { char id[UMI_FC_ID_CAPACITY]; double values[UMI_FC_MAX_LAYOUT_VALUES]; size_t count; uint64_t revision; uint64_t fingerprint; } UmiFcLayoutSnapshot;
void umi_fc_layout_snapshot_init(UmiFcLayoutSnapshot *snapshot,const char *id);
UmiStatus umi_fc_layout_snapshot_append(UmiFcLayoutSnapshot *snapshot,double value);
uint64_t umi_fc_layout_snapshot_fingerprint(UmiFcLayoutSnapshot *snapshot);

#ifdef __cplusplus
}
#endif
#endif
