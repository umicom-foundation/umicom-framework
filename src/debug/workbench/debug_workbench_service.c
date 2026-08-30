/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_workbench_service.c
 *
 * PURPOSE:
 *   Compose canonical debug runtime state into one toolkit-neutral debugger workbench service.
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
#include "umicom/debug/workbench/debug_workbench_service.h"
#include <string.h>

void umi_debug_workbench_debug_workbench_service_init(UmiDebugWorkbenchDebugWorkbenchService *service)
{
    if (service == NULL) return;
    memset(service, 0, sizeof *service);
    umi_debug_workbench_debug_session_registry_init(&service->sessions);
    umi_debug_workbench_breakpoint_collection_init(&service->breakpoints);
    umi_debug_workbench_thread_group_init(&service->threads);
    umi_debug_workbench_watch_expression_collection_init(&service->watches);
    service->revision = 1U;
}

UmiStatus umi_debug_workbench_debug_workbench_service_open_session(UmiDebugWorkbenchDebugWorkbenchService *service, const UmiDebugWorkbenchEntry *session)
{
    UmiStatus status;
    if (service == NULL || !umi_debug_workbench_entry_valid(session)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_debug_workbench_debug_session_registry_add(&service->sessions, session);
    if (status != UMI_STATUS_OK) return status;
    status = umi_debug_workbench_debug_session_registry_select(&service->sessions, session->id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_debug_workbench_debug_status_model_init(&service->status, session->id);
    if (status != UMI_STATUS_OK) return status;
    service->status_initialized = true;
    service->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_debug_workbench_service_select_session(UmiDebugWorkbenchDebugWorkbenchService *service, const char *session_id)
{
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_debug_workbench_debug_session_registry_select(&service->sessions, session_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_debug_workbench_debug_status_model_init(&service->status, session_id);
    if (status != UMI_STATUS_OK) return status;
    service->status_initialized = true;
    service->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_debug_workbench_service_refresh_status(UmiDebugWorkbenchDebugWorkbenchService *service, UmiDebugWorkbenchSessionPhase phase, const char *stop_reason, uint32_t frame_count, uint32_t variable_count)
{
    if (service == NULL || !service->status_initialized) return UMI_STATUS_INVALID_STATE;
    if (umi_debug_workbench_debug_status_model_update(&service->status, phase, stop_reason, (uint32_t)service->threads.count, frame_count, variable_count) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    service->revision++;
    return UMI_STATUS_OK;
}

int umi_debug_workbench_debug_workbench_service_valid(const UmiDebugWorkbenchDebugWorkbenchService *service)
{
    if (service == NULL || service->revision == 0U) return 0;
    if (!umi_debug_workbench_debug_session_registry_valid(&service->sessions)) return 0;
    if (!umi_debug_workbench_breakpoint_collection_valid(&service->breakpoints)) return 0;
    if (!umi_debug_workbench_thread_group_valid(&service->threads)) return 0;
    if (!umi_debug_workbench_watch_expression_collection_valid(&service->watches)) return 0;
    if (service->status_initialized && !umi_debug_workbench_debug_status_model_valid(&service->status)) return 0;
    return 1;
}
