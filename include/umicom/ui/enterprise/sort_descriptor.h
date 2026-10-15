/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/sort_descriptor.h
 *
 * PURPOSE:
 *   Describe one deterministic multi-column sort key.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_SORT_DESCRIPTOR_H
#define UMICOM_UI_ENTERPRISE_SORT_DESCRIPTOR_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent sort descriptor data shared with callers of this public contract.
 */
typedef struct UmiUiEntSortDescriptor {
    char column_id[UMI_UI_ENT_ID_CAPACITY];
    UmiUiEntSortDirection direction;
    int32_t priority;
    int case_sensitive;
} UmiUiEntSortDescriptor;
/**
 * Initialise ui ent sort descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_sort_descriptor_init(UmiUiEntSortDescriptor *value);
/**
 * Check that ui ent sort descriptor satisfies its contract before another service relies
 * on it.
 */
int umi_ui_ent_sort_descriptor_validate(const UmiUiEntSortDescriptor *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_ent_sort_descriptor_archive_encode(const UmiUiEntSortDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_ent_sort_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiUiEntSortDescriptor *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ui_ent_sort_descriptor_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUiEntSortDescriptor *expected, UmiUiEntSortDescriptor *value);


#ifdef __cplusplus
}
#endif

#endif
