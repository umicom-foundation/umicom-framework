/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/accessibility_node.h
 *
 * PURPOSE:
 *   Define the reusable context-link accessibility node contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_ACCESSIBILITY_NODE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_ACCESSIBILITY_NODE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link accessibility node data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkAccessibilityNode {
    uint32_t structure_size;
    char node_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char role[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkAccessibilityNode;

/**
 * Initialise workbench context link accessibility node from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_link_accessibility_node_init(UmiWorkbenchContextLinkAccessibilityNode *record,
                                           const char *identity);
/**
 * Check that workbench context link accessibility node satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_link_accessibility_node_validate(
    const UmiWorkbenchContextLinkAccessibilityNode *record);
/**
 * Copy workbench context link accessibility node into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_accessibility_node_copy(
    UmiWorkbenchContextLinkAccessibilityNode *destination,
    const UmiWorkbenchContextLinkAccessibilityNode *source);
/**
 * Provide the workbench context link accessibility node hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_link_accessibility_node_hash(
    const UmiWorkbenchContextLinkAccessibilityNode *record);
/**
 * Provide the workbench context link accessibility node set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_accessibility_node_set_primary(
    UmiWorkbenchContextLinkAccessibilityNode *record,
    const char *value);
/**
 * Provide the workbench context link accessibility node set secondary operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_link_accessibility_node_set_secondary(
    UmiWorkbenchContextLinkAccessibilityNode *record,
    const char *value);
/**
 * Provide the workbench context link accessibility node touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_link_accessibility_node_touch(
    UmiWorkbenchContextLinkAccessibilityNode *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
