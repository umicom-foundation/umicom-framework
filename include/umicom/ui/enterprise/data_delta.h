/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/data_delta.h
 *
 * PURPOSE:
 *   Represent inserted, removed, updated or reset row ranges.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_DATA_DELTA_H
#define UMICOM_UI_ENTERPRISE_DATA_DELTA_H

#include "umicom/ui/enterprise/types.h"
#include "umicom/base/value_archive.h"
/* Public C functions retain C linkage when included by an embedding host. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named ui ent delta kind values accepted by this public contract.
 */
typedef enum UmiUiEntDeltaKind { UMI_UI_ENT_DELTA_INSERT=1, UMI_UI_ENT_DELTA_REMOVE=2, UMI_UI_ENT_DELTA_UPDATE=3, UMI_UI_ENT_DELTA_RESET=4 } UmiUiEntDeltaKind;
/**
 * Represent the ui ent data delta data shared with callers of this public contract.
 */
typedef struct UmiUiEntDataDelta { UmiUiEntDeltaKind kind; UmiUiEntSpan rows; uint64_t sequence; } UmiUiEntDataDelta;
/**
 * Check that ui ent data delta satisfies its contract before another service relies on it.
 */
int umi_ui_ent_data_delta_validate(const UmiUiEntDataDelta *delta);
/**
 * Provide the ui ent data delta touches operation used by this module and its client
 * applications.
 */
int umi_ui_ent_data_delta_touches(const UmiUiEntDataDelta *delta,size_t row);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_ent_data_delta_archive_encode(const UmiUiEntDataDelta *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_ent_data_delta_archive_decode(const void *bytes, size_t byte_count,
    UmiUiEntDataDelta *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ui_ent_data_delta_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUiEntDataDelta *expected, UmiUiEntDataDelta *value);


#ifdef __cplusplus
}
#endif
#endif
