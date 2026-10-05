/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/frozen_columns.h
 *
 * PURPOSE:
 *   Describe leading and trailing frozen-column regions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_FROZEN_COLUMNS_H
#define UMICOM_UI_ENTERPRISE_FROZEN_COLUMNS_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent frozen columns data shared with callers of this public contract.
 */
typedef struct UmiUiEntFrozenColumns {
    size_t leading_count;
    size_t trailing_count;
    size_t total_columns;
} UmiUiEntFrozenColumns;
/**
 * Initialise ui ent frozen columns from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_ent_frozen_columns_init(UmiUiEntFrozenColumns *value);
/**
 * Check that ui ent frozen columns satisfies its contract before another service relies on
 * it.
 */
int umi_ui_ent_frozen_columns_validate(const UmiUiEntFrozenColumns *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_ent_frozen_columns_archive_encode(const UmiUiEntFrozenColumns *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_ent_frozen_columns_archive_decode(const void *bytes, size_t byte_count,
    UmiUiEntFrozenColumns *value);

#ifdef __cplusplus
}
#endif

#endif
