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

/**
 * Represent the workbench layout service config data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutServiceConfig {
    uint32_t structure_size;
    UmiWorkbenchLayoutAccessPolicy access_policy;
    UmiWorkbenchLayoutValidationOptions validation;
    bool seed_framework_templates;
    bool maintain_history;
    bool emit_events;
    bool auto_checkpoint_sessions;
} UmiWorkbenchLayoutServiceConfig;

/**
 * Represent the workbench layout service snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the workbench layout service data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchLayoutService UmiWorkbenchLayoutService;

/**
 * Provide the workbench layout service config default operation used by this module and
 * its client applications.
 */
UmiWorkbenchLayoutServiceConfig
umi_workbench_layout_service_config_default(void);

/**
 * Initialise workbench layout service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_layout_service_create(
    const UmiWorkbenchLayoutServiceConfig *config,
    const UmiWorkbenchLayoutStoreAdapter *store_adapter,
    UmiWorkbenchLayoutService **out_service);

/**
 * Release or reset state held by workbench layout service so the same storage can be
 * reused safely.
 */
void umi_workbench_layout_service_destroy(
    UmiWorkbenchLayoutService *service);

/**
 * Provide the workbench layout service start operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_service_start(
    UmiWorkbenchLayoutService *service);

/**
 * Provide the workbench layout service stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_service_stop(
    UmiWorkbenchLayoutService *service,
    uint64_t timestamp_ms);

/**
 * Provide the workbench layout service create layout operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_service_create_layout(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutIdentity *identity,
    const char *name,
    UmiWorkbenchLayoutDocument *out_document);

/**
 * Provide the workbench layout service clone template operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_service_clone_template(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *template_id,
    const UmiWorkbenchLayoutIdentity *identity,
    const char *name,
    UmiWorkbenchLayoutDocument *out_document);

/**
 * Read workbench layout service into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_workbench_layout_service_load(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *layout_id);

/**
 * Provide the workbench layout service activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_service_activate(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *layout_id);

/**
 * Provide the workbench layout service apply operation operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_service_apply_operation(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperationResult *out_result);

/**
 * Write workbench layout service in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_workbench_layout_service_save(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *actor_id,
    const char *correlation_id,
    uint64_t timestamp_ms,
    UmiWorkbenchLayoutPersistenceResult *out_result);

/**
 * Provide the workbench layout service delete operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_service_delete(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *layout_id,
    uint64_t expected_revision);

/**
 * Provide the workbench layout service undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_service_undo(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal);

/**
 * Provide the workbench layout service redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_service_redo(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal);

/**
 * Provide the workbench layout service begin session operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_service_begin_session(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutSession *session);

/**
 * Provide the workbench layout service checkpoint session operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_service_checkpoint_session(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    uint64_t timestamp_ms,
    UmiWorkbenchLayoutPersistenceResult *out_result);

/**
 * Provide the workbench layout service browser search operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_service_browser_search(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutBrowserQuery *query,
    UmiWorkbenchLayoutBrowserResult *out_result);

/**
 * Provide the workbench layout service snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_service_snapshot(
    const UmiWorkbenchLayoutService *service,
    UmiWorkbenchLayoutServiceSnapshot *out_snapshot);

/**
 * Provide the workbench layout service active layout operation used by this module and its
 * client applications.
 */
const UmiWorkbenchLayoutDocument *
umi_workbench_layout_service_active_layout(
    const UmiWorkbenchLayoutService *service);

/**
 * Provide the workbench layout service active layout mutable operation used by this module
 * and its client applications.
 */
UmiWorkbenchLayoutDocument *
umi_workbench_layout_service_active_layout_mutable(
    UmiWorkbenchLayoutService *service);

/**
 * Provide the workbench layout service active session operation used by this module and
 * its client applications.
 */
const UmiWorkbenchLayoutSession *
umi_workbench_layout_service_active_session(
    const UmiWorkbenchLayoutService *service);

/**
 * Provide the workbench layout service templates operation used by this module and its
 * client applications.
 */
UmiWorkbenchLayoutTemplateRegistry *
umi_workbench_layout_service_templates(
    UmiWorkbenchLayoutService *service);

/**
 * Provide the workbench layout service panels operation used by this module and its client
 * applications.
 */
UmiWorkbenchPanelRegistry *
umi_workbench_layout_service_panels(
    UmiWorkbenchLayoutService *service);

/**
 * Provide the workbench layout service perspectives operation used by this module and its
 * client applications.
 */
UmiWorkbenchPerspectiveRegistry *
umi_workbench_layout_service_perspectives(
    UmiWorkbenchLayoutService *service);

/**
 * Provide the workbench layout service events operation used by this module and its client
 * applications.
 */
UmiWorkbenchLayoutEventBus *
umi_workbench_layout_service_events(
    UmiWorkbenchLayoutService *service);

#ifdef __cplusplus
}
#endif

#endif
