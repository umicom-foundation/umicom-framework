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

/**
 * Represent the workbench context link publish request data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise workbench context link publish request from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_publish_request_init(UmiWorkbenchContextLinkPublishRequest *record,
                                           const char *identity);
/**
 * Check that workbench context link publish request satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_publish_request_validate(
    const UmiWorkbenchContextLinkPublishRequest *record);
/**
 * Copy workbench context link publish request into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_publish_request_copy(
    UmiWorkbenchContextLinkPublishRequest *destination,
    const UmiWorkbenchContextLinkPublishRequest *source);
/**
 * Provide the workbench context link publish request hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_link_publish_request_hash(
    const UmiWorkbenchContextLinkPublishRequest *record);
/**
 * Provide the workbench context link publish request set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_publish_request_set_primary(
    UmiWorkbenchContextLinkPublishRequest *record,
    const char *value);
/**
 * Provide the workbench context link publish request set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_publish_request_set_secondary(
    UmiWorkbenchContextLinkPublishRequest *record,
    const char *value);
/**
 * Provide the workbench context link publish request touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_publish_request_touch(
    UmiWorkbenchContextLinkPublishRequest *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
