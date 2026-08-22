/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/accessibility_node.h
 *
 * PURPOSE:
 *   Define the reusable context-link accessibility node contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_ACCESSIBILITY_NODE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_ACCESSIBILITY_NODE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_context_link_accessibility_node_init(UmiWorkbenchContextLinkAccessibilityNode *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_accessibility_node_validate(
    const UmiWorkbenchContextLinkAccessibilityNode *record);
UmiStatus umi_workbench_context_link_accessibility_node_copy(
    UmiWorkbenchContextLinkAccessibilityNode *destination,
    const UmiWorkbenchContextLinkAccessibilityNode *source);
uint64_t umi_workbench_context_link_accessibility_node_hash(
    const UmiWorkbenchContextLinkAccessibilityNode *record);
UmiStatus umi_workbench_context_link_accessibility_node_set_primary(
    UmiWorkbenchContextLinkAccessibilityNode *record,
    const char *value);
UmiStatus umi_workbench_context_link_accessibility_node_set_secondary(
    UmiWorkbenchContextLinkAccessibilityNode *record,
    const char *value);
void umi_workbench_context_link_accessibility_node_touch(
    UmiWorkbenchContextLinkAccessibilityNode *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
