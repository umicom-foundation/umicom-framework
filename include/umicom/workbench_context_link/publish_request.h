/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/publish_request.h
 *
 * PURPOSE:
 *   Define the reusable context publication request contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PUBLISH_REQUEST_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PUBLISH_REQUEST_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkPublishRequest {
    uint32_t structure_size;
    char request_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char source_panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkPublishRequest;

void umi_workbench_context_link_publish_request_init(UmiWorkbenchContextLinkPublishRequest *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_publish_request_validate(
    const UmiWorkbenchContextLinkPublishRequest *record);
UmiStatus umi_workbench_context_link_publish_request_copy(
    UmiWorkbenchContextLinkPublishRequest *destination,
    const UmiWorkbenchContextLinkPublishRequest *source);
uint64_t umi_workbench_context_link_publish_request_hash(
    const UmiWorkbenchContextLinkPublishRequest *record);
UmiStatus umi_workbench_context_link_publish_request_set_primary(
    UmiWorkbenchContextLinkPublishRequest *record,
    const char *value);
UmiStatus umi_workbench_context_link_publish_request_set_secondary(
    UmiWorkbenchContextLinkPublishRequest *record,
    const char *value);
void umi_workbench_context_link_publish_request_touch(
    UmiWorkbenchContextLinkPublishRequest *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
