/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/workbench_context_host.h
 *
 * PURPOSE:
 *   Expose the complete toolkit-neutral Workbench Context Host platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_WORKBENCH_CONTEXT_HOST_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_WORKBENCH_CONTEXT_HOST_H

#include "umicom/workbench_context_host/account_publisher.h"
#include "umicom/workbench_context_host/ai_publisher.h"
#include "umicom/workbench_context_host/application_publisher.h"
#include "umicom/workbench_context_host/audit.h"
#include "umicom/workbench_context_host/auto_link_policy.h"
#include "umicom/workbench_context_host/command.h"
#include "umicom/workbench_context_host/compatibility.h"
#include "umicom/workbench_context_host/controller.h"
#include "umicom/workbench_context_host/delivery.h"
#include "umicom/workbench_context_host/delivery_policy.h"
#include "umicom/workbench_context_host/diagnostic_publisher.h"
#include "umicom/workbench_context_host/diagnostics.h"
#include "umicom/workbench_context_host/dispatcher.h"
#include "umicom/workbench_context_host/endpoint.h"
#include "umicom/workbench_context_host/endpoint_projection.h"
#include "umicom/workbench_context_host/endpoint_registry.h"
#include "umicom/workbench_context_host/event.h"
#include "umicom/workbench_context_host/event_queue.h"
#include "umicom/workbench_context_host/experience_policy.h"
#include "umicom/workbench_context_host/experience_profile.h"
#include "umicom/workbench_context_host/experience_profile_catalogue.h"
#include "umicom/workbench_context_host/focus_propagation.h"
#include "umicom/workbench_context_host/group_definition.h"
#include "umicom/workbench_context_host/group_picker_projection.h"
#include "umicom/workbench_context_host/health.h"
#include "umicom/workbench_context_host/history_projection.h"
#include "umicom/workbench_context_host/host.h"
#include "umicom/workbench_context_host/inbox.h"
#include "umicom/workbench_context_host/inbox_projection.h"
#include "umicom/workbench_context_host/inbox_registry.h"
#include "umicom/workbench_context_host/inspector_projection.h"
#include "umicom/workbench_context_host/instrument_publisher.h"
#include "umicom/workbench_context_host/media_publisher.h"
#include "umicom/workbench_context_host/metrics.h"
#include "umicom/workbench_context_host/metrics_projection.h"
#include "umicom/workbench_context_host/observation.h"
#include "umicom/workbench_context_host/observer.h"
#include "umicom/workbench_context_host/payload.h"
#include "umicom/workbench_context_host/profile.h"
#include "umicom/workbench_context_host/profile_apply.h"
#include "umicom/workbench_context_host/profile_catalogue.h"
#include "umicom/workbench_context_host/project_publisher.h"
#include "umicom/workbench_context_host/publication_guard.h"
#include "umicom/workbench_context_host/query.h"
#include "umicom/workbench_context_host/route_preview.h"
#include "umicom/workbench_context_host/scope_policy.h"
#include "umicom/workbench_context_host/search.h"
#include "umicom/workbench_context_host/selection_publisher.h"
#include "umicom/workbench_context_host/session.h"
#include "umicom/workbench_context_host/session_service.h"
#include "umicom/workbench_context_host/snapshot.h"
#include "umicom/workbench_context_host/source_control_publisher.h"
#include "umicom/workbench_context_host/source_location_publisher.h"
#include "umicom/workbench_context_host/status_projection.h"
#include "umicom/workbench_context_host/terminal_publisher.h"
#include "umicom/workbench_context_host/test_publisher.h"
#include "umicom/workbench_context_host/throttle.h"
#include "umicom/workbench_context_host/toolbar_projection.h"
#include "umicom/workbench_context_host/trade_publisher.h"
#include "umicom/workbench_context_host/types.h"
#include "umicom/workbench_context_host/workbench_adapter.h"
#include "umicom/workbench_context_host/workspace_publisher.h"

#endif
