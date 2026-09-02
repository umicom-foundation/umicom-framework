/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/service.h
 *
 * PURPOSE:
 *   Own source registration, event queueing, coalescing and publication into the Workbench Context Host.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_SERVICE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_SERVICE_H

#include "umicom/workbench_context_event/coalescer.h"
#include "umicom/workbench_context_event/payload_mapper.h"
#include "umicom/workbench_context_event/queue.h"
#include "umicom/workbench_context_event/source_registry.h"
#include "umicom/workbench_context_host/host.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event service metrics data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventServiceMetrics {
    uint64_t submitted_count;
    uint64_t accepted_count;
    uint64_t coalesced_count;
    uint64_t routed_count;
    uint64_t rejected_count;
    uint64_t dropped_count;
    uint64_t mapping_failure_count;
    uint64_t publication_failure_count;
    uint64_t revision;
} UmiWorkbenchContextEventServiceMetrics;

/**
 * Represent the workbench context event service data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextEventService {
    UmiWorkbenchContextHost *host;
    UmiWorkbenchContextEventSourceRegistry sources;
    UmiWorkbenchContextEventQueue queue;
    UmiWorkbenchContextEventCoalescer coalescer;
    UmiWorkbenchContextEventServiceMetrics metrics;
    uint64_t sequence;
    uint64_t revision;
    bool suspended;
} UmiWorkbenchContextEventService;

/**
 * Initialise workbench context event service from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_context_event_service_create(
    UmiWorkbenchContextHost *host,
    UmiWorkbenchContextEventService **out_service);
/**
 * Release or reset state held by workbench context event service so the same storage can
 * be reused safely.
 */
void umi_workbench_context_event_service_destroy(
    UmiWorkbenchContextEventService *service);
/**
 * Provide the workbench context event service register source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_service_register_source(
    UmiWorkbenchContextEventService *service,
    const UmiWorkbenchContextEventSourceDescriptor *descriptor);
/**
 * Provide the workbench context event service submit operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_service_submit(
    UmiWorkbenchContextEventService *service,
    UmiWorkbenchContextEvent *event);
/**
 * Provide the workbench context event service pump operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_service_pump(
    UmiWorkbenchContextEventService *service,
    size_t max_events,
    size_t *out_processed);
/**
 * Provide the workbench context event service set suspended operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_service_set_suspended(
    UmiWorkbenchContextEventService *service,
    bool suspended);

#ifdef __cplusplus
}
#endif
#endif
