/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/permission.h
 *
 * PURPOSE:
 *   Define the reusable context-link permission record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PERMISSION_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PERMISSION_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link permission data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkPermission {
    uint32_t structure_size;
    char permission_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char action_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    UmiContextKind context_kind;
    UmiContextChannelColour colour;
    UmiWorkbenchContextLinkMode mode;
    UmiWorkbenchContextLinkState state;
    UmiWorkbenchContextLinkOrigin origin;
    UmiWorkbenchContextLinkPriority priority;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextLinkPermission;

/**
 * Initialise workbench context link permission from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_permission_init(UmiWorkbenchContextLinkPermission *record,
                                           const char *identity);
/**
 * Check that workbench context link permission satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_permission_validate(
    const UmiWorkbenchContextLinkPermission *record);
/**
 * Copy workbench context link permission into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_permission_copy(
    UmiWorkbenchContextLinkPermission *destination,
    const UmiWorkbenchContextLinkPermission *source);
/**
 * Provide the workbench context link permission hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_permission_hash(
    const UmiWorkbenchContextLinkPermission *record);
/**
 * Provide the workbench context link permission set primary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_permission_set_primary(
    UmiWorkbenchContextLinkPermission *record,
    const char *value);
/**
 * Provide the workbench context link permission set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_permission_set_secondary(
    UmiWorkbenchContextLinkPermission *record,
    const char *value);
/**
 * Provide the workbench context link permission touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_permission_touch(
    UmiWorkbenchContextLinkPermission *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
