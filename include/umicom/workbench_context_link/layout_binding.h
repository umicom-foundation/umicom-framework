/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/layout_binding.h
 *
 * PURPOSE:
 *   Define the reusable layout context binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_LAYOUT_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_LAYOUT_BINDING_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link layout binding data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkLayoutBinding {
    uint32_t structure_size;
    char binding_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char layout_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkLayoutBinding;

/**
 * Initialise workbench context link layout binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_layout_binding_init(UmiWorkbenchContextLinkLayoutBinding *record,
                                           const char *identity);
/**
 * Check that workbench context link layout binding satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_layout_binding_validate(
    const UmiWorkbenchContextLinkLayoutBinding *record);
/**
 * Copy workbench context link layout binding into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_layout_binding_copy(
    UmiWorkbenchContextLinkLayoutBinding *destination,
    const UmiWorkbenchContextLinkLayoutBinding *source);
/**
 * Provide the workbench context link layout binding hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_layout_binding_hash(
    const UmiWorkbenchContextLinkLayoutBinding *record);
/**
 * Provide the workbench context link layout binding set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_layout_binding_set_primary(
    UmiWorkbenchContextLinkLayoutBinding *record,
    const char *value);
/**
 * Provide the workbench context link layout binding set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_layout_binding_set_secondary(
    UmiWorkbenchContextLinkLayoutBinding *record,
    const char *value);
/**
 * Provide the workbench context link layout binding touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_layout_binding_touch(
    UmiWorkbenchContextLinkLayoutBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
