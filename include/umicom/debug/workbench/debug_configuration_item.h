/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_configuration_item.h
 *
 * PURPOSE:
 *   Represent one launch or attach configuration surfaced by the workbench.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_CONFIGURATION_ITEM_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_CONFIGURATION_ITEM_H

#include "umicom/debug/workbench/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug configuration item data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugConfigurationItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugConfigurationItem;

/**
 * Initialise debug workbench debug configuration item from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_configuration_item_init(UmiDebugWorkbenchDebugConfigurationItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench debug configuration item set state operation used by this
 * module and its client applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: value.revision, revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_debug_workbench_debug_configuration_item_set_state(UmiDebugWorkbenchDebugConfigurationItem *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench debug configuration item set while leaving the underlying catalogue
 * or model owned by this module.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_debug_workbench_debug_configuration_item_set_selected(UmiDebugWorkbenchDebugConfigurationItem *model, bool selected);
/**
 * Provide the debug workbench debug configuration item set enabled operation used by this
 * module and its client applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_debug_workbench_debug_configuration_item_set_enabled(UmiDebugWorkbenchDebugConfigurationItem *model, bool enabled);
/**
 * Check that debug workbench debug configuration item satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_debug_configuration_item_valid(const UmiDebugWorkbenchDebugConfigurationItem *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_debug_workbench_debug_configuration_item_archive_encode(const UmiDebugWorkbenchDebugConfigurationItem *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_debug_workbench_debug_configuration_item_archive_decode(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchDebugConfigurationItem *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiDebugWorkbenchDebugConfigurationItem object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_debug_workbench_debug_configuration_item_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchDebugConfigurationItem *workspace, UmiDebugWorkbenchDebugConfigurationItem *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_debug_workbench_debug_configuration_item_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDebugWorkbenchDebugConfigurationItem *expected, UmiDebugWorkbenchDebugConfigurationItem *value);


#ifdef __cplusplus
}
#endif
#endif
