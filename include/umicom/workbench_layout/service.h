/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/service.h
 *
 * PURPOSE:
 *   Coordinate layout documents, templates, panels, perspectives, history, persistence, recovery, browser queries and events as one Framework service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_SERVICE_H
#define UMICOM_WORKBENCH_LAYOUT_SERVICE_H

#include "umicom/workbench_layout/access_policy.h"
#include "umicom/workbench_layout/browser.h"
#include "umicom/workbench_layout/events.h"
#include "umicom/workbench_layout/history.h"
#include "umicom/workbench_layout/panel_registry.h"
#include "umicom/workbench_layout/persistence.h"
#include "umicom/workbench_layout/perspective_registry.h"
#include "umicom/workbench_layout/template_registry.h"
#include "umicom/workbench_layout/transaction.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchLayoutServiceConfig {
    uint32_t structure_size;
    UmiWorkbenchLayoutAccessPolicy access_policy;
    UmiWorkbenchLayoutValidationOptions validation;
    bool seed_framework_templates;
    bool maintain_history;
    bool emit_events;
    bool auto_checkpoint_sessions;
} UmiWorkbenchLayoutServiceConfig;

typedef struct UmiWorkbenchLayoutServiceSnapshot {
    uint32_t structure_size;
    size_t loaded_layout_count;
    size_t template_count;
    size_t panel_count;
    size_t perspective_count;
    size_t history_entry_count;
    size_t event_subscription_count;
    char active_layout_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char active_session_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    uint64_t active_layout_revision;
    uint64_t service_revision;
    bool dirty;
    bool recovery_available;
} UmiWorkbenchLayoutServiceSnapshot;

typedef struct UmiWorkbenchLayoutService UmiWorkbenchLayoutService;

UmiWorkbenchLayoutServiceConfig
umi_workbench_layout_service_config_default(void);

UmiStatus umi_workbench_layout_service_create(
    const UmiWorkbenchLayoutServiceConfig *config,
    const UmiWorkbenchLayoutStoreAdapter *store_adapter,
    UmiWorkbenchLayoutService **out_service);

void umi_workbench_layout_service_destroy(
    UmiWorkbenchLayoutService *service);

UmiStatus umi_workbench_layout_service_start(
    UmiWorkbenchLayoutService *service);

UmiStatus umi_workbench_layout_service_stop(
    UmiWorkbenchLayoutService *service,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_layout_service_create_layout(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutIdentity *identity,
    const char *name,
    UmiWorkbenchLayoutDocument *out_document);

UmiStatus umi_workbench_layout_service_clone_template(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *template_id,
    const UmiWorkbenchLayoutIdentity *identity,
    const char *name,
    UmiWorkbenchLayoutDocument *out_document);

UmiStatus umi_workbench_layout_service_load(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *layout_id);

UmiStatus umi_workbench_layout_service_activate(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *layout_id);

UmiStatus umi_workbench_layout_service_apply_operation(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperationResult *out_result);

UmiStatus umi_workbench_layout_service_save(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *actor_id,
    const char *correlation_id,
    uint64_t timestamp_ms,
    UmiWorkbenchLayoutPersistenceResult *out_result);

UmiStatus umi_workbench_layout_service_delete(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *layout_id,
    uint64_t expected_revision);

UmiStatus umi_workbench_layout_service_undo(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal);

UmiStatus umi_workbench_layout_service_redo(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal);

UmiStatus umi_workbench_layout_service_begin_session(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutSession *session);

UmiStatus umi_workbench_layout_service_checkpoint_session(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    uint64_t timestamp_ms,
    UmiWorkbenchLayoutPersistenceResult *out_result);

UmiStatus umi_workbench_layout_service_browser_search(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutBrowserQuery *query,
    UmiWorkbenchLayoutBrowserResult *out_result);

UmiStatus umi_workbench_layout_service_snapshot(
    const UmiWorkbenchLayoutService *service,
    UmiWorkbenchLayoutServiceSnapshot *out_snapshot);

const UmiWorkbenchLayoutDocument *
umi_workbench_layout_service_active_layout(
    const UmiWorkbenchLayoutService *service);

UmiWorkbenchLayoutDocument *
umi_workbench_layout_service_active_layout_mutable(
    UmiWorkbenchLayoutService *service);

const UmiWorkbenchLayoutSession *
umi_workbench_layout_service_active_session(
    const UmiWorkbenchLayoutService *service);

UmiWorkbenchLayoutTemplateRegistry *
umi_workbench_layout_service_templates(
    UmiWorkbenchLayoutService *service);

UmiWorkbenchPanelRegistry *
umi_workbench_layout_service_panels(
    UmiWorkbenchLayoutService *service);

UmiWorkbenchPerspectiveRegistry *
umi_workbench_layout_service_perspectives(
    UmiWorkbenchLayoutService *service);

UmiWorkbenchLayoutEventBus *
umi_workbench_layout_service_events(
    UmiWorkbenchLayoutService *service);

#ifdef __cplusplus
}
#endif

#endif
