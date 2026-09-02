/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/layout_snapshot.h
 *
 * PURPOSE:
 *   Define immutable layout snapshot metadata used by persistence, autosave, recovery and visual history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_LAYOUT_SNAPSHOT_H
#define UMICOM_UI_WORKSTATION_LAYOUT_SNAPSHOT_H

#include <stdint.h>
#include "umicom/ui/workstation/layout_template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws layout snapshot data shared with callers of this public contract.
 */
typedef struct UmiWsLayoutSnapshot {
    char snapshot_id[UMI_UI_ID_CAPACITY];
    char layout_id[UMI_UI_ID_CAPACITY];
    uint64_t layout_fingerprint;
    uint64_t sequence;
    int64_t timestamp_ms;
    bool autosave;
} UmiWsLayoutSnapshot;

/**
 * Provide the ws layout snapshot capture operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_layout_snapshot_capture(UmiWsLayoutSnapshot *snapshot,
                                         const char *snapshot_id,
                                         const UmiWsLayoutTemplate *layout,
                                         uint64_t sequence,
                                         int64_t timestamp_ms,
                                         bool autosave);
/**
 * Provide the ws layout snapshot matches operation used by this module and its client
 * applications.
 */
bool umi_ws_layout_snapshot_matches(const UmiWsLayoutSnapshot *snapshot, const UmiWsLayoutTemplate *layout);

#ifdef __cplusplus
}
#endif

#endif
