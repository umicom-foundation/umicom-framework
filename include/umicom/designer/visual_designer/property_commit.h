/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/property_commit.h
 *
 * PURPOSE:
 *   Record before/after property values for review, undo and audit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_PROPERTY_COMMIT_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_PROPERTY_COMMIT_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer property commit data shared with callers of this public contract.
 */
typedef struct UmiRadPropertyCommit {
    char component_id[UMI_RAD_ID_CAPACITY];
    char property_id[UMI_RAD_ID_CAPACITY];
    char before_value[UMI_RAD_VALUE_CAPACITY];
    char after_value[UMI_RAD_VALUE_CAPACITY];
    uint64_t revision;
} UmiRadPropertyCommit;
/**
 * Initialise visual designer property commit from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_property_commit_init(UmiRadPropertyCommit *item);
/**
 * Check that visual designer property commit satisfies its contract before another service relies on
 * it.
 */
int umi_rad_property_commit_is_valid(const UmiRadPropertyCommit *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_property_commit_archive_encode(const UmiRadPropertyCommit *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_property_commit_archive_decode(const void *bytes, size_t byte_count,
    UmiRadPropertyCommit *value);

#ifdef __cplusplus
}
#endif
#endif
