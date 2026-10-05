/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/row_height.h
 *
 * PURPOSE:
 *   Describe fixed or adaptive row-height constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_ROW_HEIGHT_H
#define UMICOM_UI_ENTERPRISE_ROW_HEIGHT_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent row height data shared with callers of this public contract.
 */
typedef struct UmiUiEntRowHeight {
    int32_t preferred;
    int32_t minimum;
    int32_t maximum;
    int automatic;
} UmiUiEntRowHeight;
/**
 * Initialise ui ent row height from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_ent_row_height_init(UmiUiEntRowHeight *value);
/**
 * Check that ui ent row height satisfies its contract before another service relies on it.
 */
int umi_ui_ent_row_height_validate(const UmiUiEntRowHeight *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_ent_row_height_archive_encode(const UmiUiEntRowHeight *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_ent_row_height_archive_decode(const void *bytes, size_t byte_count,
    UmiUiEntRowHeight *value);

#ifdef __cplusplus
}
#endif

#endif
