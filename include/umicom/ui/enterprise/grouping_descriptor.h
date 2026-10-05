/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/grouping_descriptor.h
 *
 * PURPOSE:
 *   Describe a grouping key and default expansion semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_GROUPING_DESCRIPTOR_H
#define UMICOM_UI_ENTERPRISE_GROUPING_DESCRIPTOR_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent grouping descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiUiEntGroupingDescriptor {
    char column_id[UMI_UI_ENT_ID_CAPACITY];
    int32_t level;
    int expanded_by_default;
} UmiUiEntGroupingDescriptor;
/**
 * Initialise ui ent grouping descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_grouping_descriptor_init(UmiUiEntGroupingDescriptor *value);
/**
 * Check that ui ent grouping descriptor satisfies its contract before another service
 * relies on it.
 */
int umi_ui_ent_grouping_descriptor_validate(const UmiUiEntGroupingDescriptor *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_ent_grouping_descriptor_archive_encode(const UmiUiEntGroupingDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_ent_grouping_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiUiEntGroupingDescriptor *value);

#ifdef __cplusplus
}
#endif

#endif
