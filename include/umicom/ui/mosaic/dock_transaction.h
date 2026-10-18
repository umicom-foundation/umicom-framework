/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/dock_transaction.h
 *
 * PURPOSE:
 *   Represent reviewable dock mutations so layout edits can be validated, committed or cancelled atomically.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_DOCK_TRANSACTION_H
#define UMICOM_UI_MOSAIC_DOCK_TRANSACTION_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic dock transaction data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicDockTransaction {
    char source_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char target_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicDockZone zone;
    uint32_t sequence;
    bool allowed;
} UmiUiMosaicDockTransaction;

/* Initializes one dock transaction relation. */
void umi_ui_mosaic_dock_transaction_init(UmiUiMosaicDockTransaction *value);
/* Configures source/target/zone without performing a renderer mutation. */
UmiStatus umi_ui_mosaic_dock_transaction_set(UmiUiMosaicDockTransaction *value, const char *source_id, const char *target_id, UmiUiMosaicDockZone zone);
/* Rejects malformed or self-referential dock operations. */
UmiStatus umi_ui_mosaic_dock_transaction_validate(const UmiUiMosaicDockTransaction *value);
/* Returns whether the relation is a centre/tab-style target. */
int umi_ui_mosaic_dock_transaction_is_centre(const UmiUiMosaicDockTransaction *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_dock_transaction_archive_encode(const UmiUiMosaicDockTransaction *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_dock_transaction_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicDockTransaction *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiUiMosaicDockTransaction object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_ui_mosaic_dock_transaction_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiUiMosaicDockTransaction *workspace, UmiUiMosaicDockTransaction *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ui_mosaic_dock_transaction_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUiMosaicDockTransaction *expected, UmiUiMosaicDockTransaction *value);


#ifdef __cplusplus
}
#endif
#endif
