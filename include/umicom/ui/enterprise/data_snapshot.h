/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/data_snapshot.h
 *
 * PURPOSE:
 *   Describe an immutable logical data snapshot used by virtual views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_DATA_SNAPSHOT_H
#define UMICOM_UI_ENTERPRISE_DATA_SNAPSHOT_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent data snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiEntDataSnapshot {
    uint64_t generation;
    size_t row_count;
    size_t column_count;
    int complete;
    uint64_t source_revision;
} UmiUiEntDataSnapshot;
/**
 * Initialise ui ent data snapshot from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_ent_data_snapshot_init(UmiUiEntDataSnapshot *value);
/**
 * Check that ui ent data snapshot satisfies its contract before another service relies on
 * it.
 */
int umi_ui_ent_data_snapshot_validate(const UmiUiEntDataSnapshot *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_ent_data_snapshot_archive_encode(const UmiUiEntDataSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_ent_data_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiUiEntDataSnapshot *value);

#ifdef __cplusplus
}
#endif

#endif
