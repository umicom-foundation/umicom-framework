/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/group_definition.h
 *
 * PURPOSE:
 *   Define a product-composition group definition applied to the canonical context-link service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_GROUP_DEFINITION_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_GROUP_DEFINITION_H

#include "umicom/workbench_context_host/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context host group definition data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostGroupDefinition {
    uint32_t structure_size;
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char title[UMI_WORKBENCH_CONTEXT_HOST_TITLE_CAPACITY];
    UmiContextChannelColour colour;
    uint64_t allowed_kinds_mask;
    UmiWorkbenchContextLinkMode default_mode;
    bool default_active;
    uint64_t revision;
} UmiWorkbenchContextHostGroupDefinition;

/**
 * Initialise workbench context host group definition from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_group_definition_init(
    UmiWorkbenchContextHostGroupDefinition *definition,
    const char *group_id);
/**
 * Check that workbench context host group definition satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_host_group_definition_validate(
    const UmiWorkbenchContextHostGroupDefinition *definition);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_host_group_definition_archive_encode(const UmiWorkbenchContextHostGroupDefinition *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_host_group_definition_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextHostGroupDefinition *value);

#ifdef __cplusplus
}
#endif
#endif
