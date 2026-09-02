/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/service.c
 *
 * PURPOSE:
 *   Coordinate semantic layout documents, templates, panels, perspectives, history, persistence, recovery, browser queries and lifecycle events as one Framework service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/service.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

struct UmiWorkbenchLayoutService {
    UmiWorkbenchLayoutServiceConfig config;
    UmiWorkbenchLayoutPersistenceService *persistence;
    UmiWorkbenchLayoutHistory *history;
    UmiWorkbenchLayoutTemplateRegistry *templates;
    UmiWorkbenchPanelRegistry *panels;
    UmiWorkbenchPerspectiveRegistry *perspectives;
    UmiWorkbenchLayoutEventBus *events;
    UmiWorkbenchRecoveryJournal *recovery;
    UmiWorkbenchLayoutDocument *active_layout;
    UmiWorkbenchLayoutSession *active_session;
    bool started;
    bool has_active_layout;
    bool has_active_session;
    uint64_t event_sequence;
    uint64_t revision;
};

/* Release or reset state held by service so the same storage can be reused safely. */
static void service_release(
    UmiWorkbenchLayoutService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) {
        return;
    }
    umi_workbench_layout_persistence_destroy(
        service->persistence);
    free(service->history);
    free(service->templates);
    free(service->panels);
    free(service->perspectives);
    free(service->events);
    free(service->recovery);
    free(service->active_layout);
    free(service->active_session);
    free(service);
}

/*
 * Provide the allocate components operation used by this module and its client
 * applications.
 */
static UmiStatus allocate_components(
    UmiWorkbenchLayoutService *service)
{
    service->history = (UmiWorkbenchLayoutHistory *)
        calloc(1U, sizeof(*service->history));
    service->templates =
        (UmiWorkbenchLayoutTemplateRegistry *)
        calloc(1U, sizeof(*service->templates));
    service->panels = (UmiWorkbenchPanelRegistry *)
        calloc(1U, sizeof(*service->panels));
    service->perspectives =
        (UmiWorkbenchPerspectiveRegistry *)
        calloc(1U, sizeof(*service->perspectives));
    service->events = (UmiWorkbenchLayoutEventBus *)
        calloc(1U, sizeof(*service->events));
    service->recovery = (UmiWorkbenchRecoveryJournal *)
        calloc(1U, sizeof(*service->recovery));
    service->active_layout =
        (UmiWorkbenchLayoutDocument *)
        calloc(1U, sizeof(*service->active_layout));
    service->active_session =
        (UmiWorkbenchLayoutSession *)
        calloc(1U, sizeof(*service->active_session));

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service->history == NULL ||
        service->templates == NULL ||
        service->panels == NULL ||
        service->perspectives == NULL ||
        service->events == NULL ||
        service->recovery == NULL ||
        service->active_layout == NULL ||
        service->active_session == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    return UMI_STATUS_OK;
}

/* Provide the make event id operation used by this module and its client applications. */
static UmiStatus make_event_id(
    UmiWorkbenchLayoutService *service,
    char *buffer,
    size_t capacity)
{
    service->event_sequence += 1U;
    return umi_workbench_layout_format(
        buffer,
        capacity,
        "layout-event-%" PRIu64,
        service->event_sequence);
}

/* Provide the publish event operation used by this module and its client applications. */
static UmiStatus publish_event(
    UmiWorkbenchLayoutService *service,
    UmiWorkbenchLayoutEventKind kind,
    const char *layout_id,
    const char *session_id,
    const char *actor_id,
    const char *correlation_id,
    const char *causation_id,
    const char *message,
    uint64_t timestamp_ms,
    uint64_t layout_revision,
    UmiStatus event_status)
{
    UmiWorkbenchLayoutEvent event;
    char event_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || !service->config.emit_events) {
        return UMI_STATUS_OK;
    }

    status = make_event_id(
        service, event_id, sizeof(event_id));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    umi_workbench_layout_event_init(
        &event, kind, event_id, layout_id);
    (void)umi_workbench_layout_copy_text(
        event.session_id,
        sizeof(event.session_id),
        session_id != NULL ? session_id : "",
        true);
    (void)umi_workbench_layout_copy_text(
        event.actor_id,
        sizeof(event.actor_id),
        actor_id != NULL ? actor_id : "",
        true);
    (void)umi_workbench_layout_copy_text(
        event.correlation_id,
        sizeof(event.correlation_id),
        correlation_id != NULL ? correlation_id : "",
        true);
    (void)umi_workbench_layout_copy_text(
        event.causation_id,
        sizeof(event.causation_id),
        causation_id != NULL ? causation_id : "",
        true);
    (void)umi_workbench_layout_copy_text(
        event.message,
        sizeof(event.message),
        message != NULL ? message : "",
        true);
    event.timestamp_ms = timestamp_ms;
    event.layout_revision = layout_revision;
    event.status = event_status;
    return umi_workbench_layout_event_bus_publish(
        service->events, &event);
}

/* Provide the decide access operation used by this module and its client applications. */
static UmiStatus decide_access(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutAccessAction action)
{
    UmiWorkbenchLayoutAccessDecision decision;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || principal == NULL ||
        document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_layout_access_decide(
        &service->config.access_policy,
        principal,
        document,
        action,
        &decision);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return decision.allowed
        ? UMI_STATUS_OK
        : UMI_STATUS_PERMISSION_DENIED;
}

/*
 * Provide the ensure create access operation used by this module and its client
 * applications.
 */
static UmiStatus ensure_create_access(
    const UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || principal == NULL ||
        principal->structure_size < sizeof(*principal) ||
        !umi_workbench_layout_text_present(principal->user_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (service->config.access_policy
            .require_trusted_workspace_for_edit &&
        !principal->trusted_workspace &&
        !principal->administrator) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (principal->administrator ||
        principal->role >= UMI_WORKBENCH_LAYOUT_ROLE_EDITOR) {
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_PERMISSION_DENIED;
}

/* Provide the make empty layout operation used by this module and its client applications. */
static UmiStatus make_empty_layout(
    const UmiWorkbenchLayoutIdentity *identity,
    const char *name,
    UmiWorkbenchLayoutDocument *document)
{
    UmiWorkbenchLayoutNode root;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (identity == NULL || document == NULL ||
        !umi_workbench_layout_text_present(identity->layout_id) ||
        !umi_workbench_layout_text_present(name)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_workbench_layout_document_init(
        document, identity->layout_id, name);
    status = umi_workbench_layout_document_set_identity(
        document, identity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    umi_workbench_layout_node_init(
        &root,
        "workbench.root",
        UMI_WORKBENCH_LAYOUT_NODE_EMPTY);
    (void)umi_workbench_layout_node_set_title(
        &root, "Empty Workbench");
    status = umi_workbench_layout_document_add_node(
        document, &root, NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_set_root(
            document, root.node_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_set_metadata(
            document,
            name,
            "user",
            "User-created semantic workbench layout.");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_set_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY,
            true);
    }
    return status;
}

/* Provide the set active layout operation used by this module and its client applications. */
static UmiStatus set_active_layout(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutDocument *document,
    const char *history_label)
{
    UmiStatus status;

    status = umi_workbench_layout_document_copy(
        service->active_layout, document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    service->has_active_layout = true;
    service->revision += 1U;

    /* Apply this branch only when its contract condition is satisfied. */
    if (service->config.maintain_history) {
        umi_workbench_layout_history_init(service->history);
        status = umi_workbench_layout_history_seed(
            service->history,
            service->active_layout,
            "history-initial",
            history_label != NULL
                ? history_label
                : "Loaded layout");
    }
    return status;
}

/* Provide the push history operation used by this module and its client applications. */
static UmiStatus push_history(
    UmiWorkbenchLayoutService *service,
    const char *entry_prefix,
    const char *label,
    const char *actor_id,
    const char *correlation_id,
    uint64_t timestamp_ms,
    bool checkpoint)
{
    char entry_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiStatus status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!service->config.maintain_history ||
        !service->has_active_layout) {
        return UMI_STATUS_OK;
    }

    status = umi_workbench_layout_format(
        entry_id,
        sizeof(entry_id),
        "%s-%" PRIu64,
        entry_prefix,
        service->revision + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    return umi_workbench_layout_history_push(
        service->history,
        service->active_layout,
        entry_id,
        label,
        actor_id,
        correlation_id,
        timestamp_ms,
        checkpoint);
}

/*
 * Provide the workbench layout service config default operation used by this module and
 * its client applications.
 */
UmiWorkbenchLayoutServiceConfig
umi_workbench_layout_service_config_default(void)
{
    UmiWorkbenchLayoutServiceConfig config;

    (void)memset(&config, 0, sizeof(config));
    config.structure_size = sizeof(config);
    config.access_policy =
        umi_workbench_layout_access_policy_default();
    config.validation =
        umi_workbench_layout_validation_options_default();
    config.seed_framework_templates = true;
    config.maintain_history = true;
    config.emit_events = true;
    config.auto_checkpoint_sessions = true;
    return config;
}

/*
 * Initialise workbench layout service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_layout_service_create(
    const UmiWorkbenchLayoutServiceConfig *config,
    const UmiWorkbenchLayoutStoreAdapter *store_adapter,
    UmiWorkbenchLayoutService **out_service)
{
    UmiWorkbenchLayoutPersistenceConfig persistence_config;
    UmiWorkbenchLayoutService *service;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store_adapter == NULL || out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;

    service = (UmiWorkbenchLayoutService *)
        calloc(1U, sizeof(*service));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    service->config = config != NULL
        ? *config
        : umi_workbench_layout_service_config_default();
    service->revision = 1U;
    service->event_sequence = 0U;

    status = allocate_components(service);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        service_release(service);
        return status;
    }

    umi_workbench_layout_history_init(service->history);
    umi_workbench_layout_template_registry_init(
        service->templates);
    umi_workbench_panel_registry_init(service->panels);
    umi_workbench_perspective_registry_init(
        service->perspectives);
    umi_workbench_layout_event_bus_init(service->events);
    umi_workbench_recovery_journal_init(service->recovery);

    /* Apply this branch only when its contract condition is satisfied. */
    if (service->config.seed_framework_templates) {
        status =
            umi_workbench_layout_template_registry_seed_framework(
                service->templates);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        service_release(service);
        return status;
    }

    persistence_config =
        umi_workbench_layout_persistence_config_default();
    status = umi_workbench_layout_persistence_create(
        store_adapter,
        &persistence_config,
        service->recovery,
        &service->persistence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        service_release(service);
        return status;
    }

    *out_service = service;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by workbench layout service so the same storage can be
 * reused safely.
 */
void umi_workbench_layout_service_destroy(
    UmiWorkbenchLayoutService *service)
{
    service_release(service);
}

/*
 * Provide the workbench layout service start operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_service_start(
    UmiWorkbenchLayoutService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (service->started) {
        return UMI_STATUS_OK;
    }
    service->started = true;
    service->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout service stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_service_stop(
    UmiWorkbenchLayoutService *service,
    uint64_t timestamp_ms)
{
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || timestamp_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!service->started) {
        return UMI_STATUS_OK;
    }

    /* Apply this operation only while the related capability or state is available. */
    if (service->has_active_session &&
        service->config.auto_checkpoint_sessions &&
        service->has_active_layout) {
        UmiWorkbenchLayoutPrincipal principal;
        UmiWorkbenchLayoutPersistenceResult result;

        (void)memset(&principal, 0, sizeof(principal));
        principal.structure_size = sizeof(principal);
        (void)umi_workbench_layout_copy_text(
            principal.user_id,
            sizeof(principal.user_id),
            service->active_session->user_id,
            false);
        (void)umi_workbench_layout_copy_text(
            principal.workspace_id,
            sizeof(principal.workspace_id),
            service->active_session->workspace_id,
            true);
        principal.role = UMI_WORKBENCH_LAYOUT_ROLE_OWNER;
        principal.trusted_workspace = true;
        principal.administrator = true;

        status =
            umi_workbench_layout_service_checkpoint_session(
                service,
                &principal,
                timestamp_ms,
                &result);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
        status =
            umi_workbench_layout_session_mark_clean_shutdown(
                service->active_session,
                timestamp_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && service->has_active_session) {
        char entry_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
        (void)umi_workbench_layout_format(
            entry_id,
            sizeof(entry_id),
            "clean-shutdown-%" PRIu64,
            service->revision + 1U);
        status =
            umi_workbench_recovery_journal_mark_clean_shutdown(
                service->recovery,
                entry_id,
                service->active_session->session_id,
                timestamp_ms);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        service->started = false;
        service->revision += 1U;
    }
    return status;
}

/*
 * Provide the workbench layout service create layout operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_service_create_layout(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutIdentity *identity,
    const char *name,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiWorkbenchLayoutDocument document;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || principal == NULL ||
        identity == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = ensure_create_access(service, principal);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = make_empty_layout(identity, name, &document);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_active_layout(
            service, &document, "Created layout");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        *out_document = document;
        (void)push_history(
            service,
            "create",
            "Created layout",
            principal->user_id,
            "",
            0U,
            true);
        (void)publish_event(
            service,
            UMI_WORKBENCH_LAYOUT_EVENT_CREATED,
            identity->layout_id,
            "",
            principal->user_id,
            "",
            "",
            "A semantic workbench layout was created.",
            0U,
            document.version.revision,
            UMI_STATUS_OK);
    }
    return status;
}

/*
 * Provide the workbench layout service clone template operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_service_clone_template(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *template_id,
    const UmiWorkbenchLayoutIdentity *identity,
    const char *name,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiWorkbenchLayoutDocument document;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || principal == NULL ||
        identity == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = ensure_create_access(service, principal);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status =
            umi_workbench_layout_template_registry_clone(
                service->templates,
                template_id,
                identity,
                name,
                &document);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_active_layout(
            service, &document, "Cloned layout template");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        *out_document = document;
        (void)push_history(
            service,
            "clone",
            "Cloned layout template",
            principal->user_id,
            "",
            0U,
            true);
        (void)publish_event(
            service,
            UMI_WORKBENCH_LAYOUT_EVENT_CREATED,
            identity->layout_id,
            "",
            principal->user_id,
            "",
            "",
            "A user layout was cloned from a template.",
            0U,
            document.version.revision,
            UMI_STATUS_OK);
    }
    return status;
}

/*
 * Read workbench layout service into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_workbench_layout_service_load(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *layout_id)
{
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchLayoutPersistenceResult result;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || principal == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_layout_persistence_load_layout(
        service->persistence,
        layout_id,
        &document,
        &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = decide_access(
            service,
            principal,
            &document,
            UMI_WORKBENCH_LAYOUT_ACCESS_VIEW);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_active_layout(
            service, &document, "Loaded layout");
    }
    return status;
}

/*
 * Provide the workbench layout service activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_service_activate(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *layout_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || principal == NULL ||
        !umi_workbench_layout_text_present(layout_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!service->has_active_layout ||
        strcmp(
            service->active_layout->identity.layout_id,
            layout_id) != 0) {
        status = umi_workbench_layout_service_load(
            service, principal, layout_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    /* Apply this operation only while the related capability or state is available. */
    if (service->has_active_session) {
        status =
            umi_workbench_layout_session_set_active_layout(
                service->active_session, layout_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    service->revision += 1U;
    return publish_event(
        service,
        UMI_WORKBENCH_LAYOUT_EVENT_ACTIVATED,
        layout_id,
        service->has_active_session
            ? service->active_session->session_id
            : "",
        principal->user_id,
        "",
        "",
        "The semantic workbench layout was activated.",
        0U,
        service->active_layout->version.revision,
        UMI_STATUS_OK);
}

/*
 * Provide the workbench layout service apply operation operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_service_apply_operation(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperationResult *out_result)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || principal == NULL ||
        operation == NULL || !service->has_active_layout) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = decide_access(
        service,
        principal,
        service->active_layout,
        UMI_WORKBENCH_LAYOUT_ACCESS_EDIT);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_operation_apply(
            service->active_layout,
            operation,
            out_result);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        service->revision += 1U;
        (void)push_history(
            service,
            "operation",
            "Applied layout operation",
            principal->user_id,
            operation->correlation_id,
            operation->timestamp_ms,
            false);
        (void)publish_event(
            service,
            UMI_WORKBENCH_LAYOUT_EVENT_UPDATED,
            service->active_layout->identity.layout_id,
            service->has_active_session
                ? service->active_session->session_id
                : "",
            principal->user_id,
            operation->correlation_id,
            operation->operation_id,
            "A semantic layout operation was applied.",
            operation->timestamp_ms,
            service->active_layout->version.revision,
            UMI_STATUS_OK);
    }
    return status;
}

/*
 * Write workbench layout service in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_workbench_layout_service_save(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *actor_id,
    const char *correlation_id,
    uint64_t timestamp_ms,
    UmiWorkbenchLayoutPersistenceResult *out_result)
{
    uint64_t expected_revision;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || principal == NULL ||
        !service->has_active_layout) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = decide_access(
        service,
        principal,
        service->active_layout,
        UMI_WORKBENCH_LAYOUT_ACCESS_EDIT);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    expected_revision =
        service->active_layout->version.base_revision;
    /* Apply this branch only when its contract condition is satisfied. */
    if (expected_revision == 0U) {
        expected_revision =
            service->active_layout->version.revision;
    }

    status = umi_workbench_layout_persistence_save_layout(
        service->persistence,
        service->active_layout,
        expected_revision,
        actor_id,
        correlation_id,
        timestamp_ms,
        out_result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND &&
        service->active_layout->version.base_revision == 0U) {
        status = umi_workbench_layout_persistence_save_layout(
            service->persistence,
            service->active_layout,
            0U,
            actor_id,
            correlation_id,
            timestamp_ms,
            out_result);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        service->active_layout->version.base_revision =
            service->active_layout->version.revision;
        (void)umi_workbench_layout_document_set_flag(
            service->active_layout,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY,
            false);
        service->revision += 1U;
        (void)push_history(
            service,
            "save",
            "Saved layout",
            actor_id,
            correlation_id,
            timestamp_ms,
            true);
        (void)publish_event(
            service,
            UMI_WORKBENCH_LAYOUT_EVENT_SAVED,
            service->active_layout->identity.layout_id,
            service->has_active_session
                ? service->active_session->session_id
                : "",
            actor_id,
            correlation_id,
            "",
            "The semantic layout was saved.",
            timestamp_ms,
            service->active_layout->version.revision,
            UMI_STATUS_OK);
    }
    return status;
}

/*
 * Provide the workbench layout service delete operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_service_delete(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const char *layout_id,
    uint64_t expected_revision)
{
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchLayoutPersistenceResult result;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || principal == NULL ||
        !umi_workbench_layout_text_present(layout_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this operation only while the related capability or state is available. */
    if (service->has_active_layout &&
        strcmp(
            service->active_layout->identity.layout_id,
            layout_id) == 0) {
        document = *service->active_layout;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = umi_workbench_layout_persistence_load_layout(
            service->persistence,
            layout_id,
            &document,
            &result);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    status = decide_access(
        service,
        principal,
        &document,
        UMI_WORKBENCH_LAYOUT_ACCESS_DELETE);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status =
            umi_workbench_layout_persistence_delete_layout(
                service->persistence,
                layout_id,
                expected_revision,
                &result);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        /* Apply this operation only while the related capability or state is available. */
        if (service->has_active_layout &&
            strcmp(
                service->active_layout->identity.layout_id,
                layout_id) == 0) {
            (void)memset(
                service->active_layout,
                0,
                sizeof(*service->active_layout));
            service->has_active_layout = false;
            umi_workbench_layout_history_init(
                service->history);
        }
        service->revision += 1U;
        (void)publish_event(
            service,
            UMI_WORKBENCH_LAYOUT_EVENT_DELETED,
            layout_id,
            "",
            principal->user_id,
            "",
            "",
            "The semantic layout was deleted.",
            0U,
            expected_revision,
            UMI_STATUS_OK);
    }
    return status;
}

/*
 * Provide the workbench layout service undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_service_undo(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || principal == NULL ||
        !service->has_active_layout) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = decide_access(
        service,
        principal,
        service->active_layout,
        UMI_WORKBENCH_LAYOUT_ACCESS_EDIT);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_history_undo(
            service->history,
            service->active_layout);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        (void)umi_workbench_layout_document_set_flag(
            service->active_layout,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY,
            true);
        service->revision += 1U;
    }
    return status;
}

/*
 * Provide the workbench layout service redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_service_redo(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || principal == NULL ||
        !service->has_active_layout) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = decide_access(
        service,
        principal,
        service->active_layout,
        UMI_WORKBENCH_LAYOUT_ACCESS_EDIT);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_history_redo(
            service->history,
            service->active_layout);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        (void)umi_workbench_layout_document_set_flag(
            service->active_layout,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY,
            true);
        service->revision += 1U;
    }
    return status;
}

/*
 * Provide the workbench layout service begin session operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_service_begin_session(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutSession *session)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_session_validate(session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    *service->active_session = *session;
    service->has_active_session = true;
    service->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout service checkpoint session operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_service_checkpoint_session(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    uint64_t timestamp_ms,
    UmiWorkbenchLayoutPersistenceResult *out_result)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || principal == NULL ||
        !service->has_active_layout ||
        !service->has_active_session) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = decide_access(
        service,
        principal,
        service->active_layout,
        UMI_WORKBENCH_LAYOUT_ACCESS_EDIT);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status =
        umi_workbench_layout_persistence_checkpoint_session(
            service->persistence,
            service->active_layout,
            service->active_session,
            principal->user_id,
            "",
            timestamp_ms,
            out_result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        service->revision += 1U;
    }
    return status;
}

/*
 * Provide the workbench layout service browser search operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_service_browser_search(
    UmiWorkbenchLayoutService *service,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutBrowserQuery *query,
    UmiWorkbenchLayoutBrowserResult *out_result)
{
    const UmiWorkbenchLayoutStoreAdapter *adapter;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || principal == NULL ||
        out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    adapter = umi_workbench_layout_persistence_adapter(
        service->persistence);
    return umi_workbench_layout_browser_search(
        adapter,
        query,
        principal->user_id,
        service->has_active_layout
            ? service->active_layout->identity.layout_id
            : "",
        out_result);
}

/*
 * Provide the workbench layout service snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_service_snapshot(
    const UmiWorkbenchLayoutService *service,
    UmiWorkbenchLayoutServiceSnapshot *out_snapshot)
{
    UmiWorkbenchRecoveryPlan plan;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = sizeof(*out_snapshot);
    out_snapshot->loaded_layout_count =
        service->has_active_layout ? 1U : 0U;
    out_snapshot->template_count = service->templates->count;
    out_snapshot->panel_count = service->panels->count;
    out_snapshot->perspective_count =
        service->perspectives->count;
    out_snapshot->history_entry_count =
        service->history->count;
    out_snapshot->event_subscription_count =
        service->events->subscription_count;
    out_snapshot->service_revision = service->revision;

    /* Apply this operation only while the related capability or state is available. */
    if (service->has_active_layout) {
        (void)umi_workbench_layout_copy_text(
            out_snapshot->active_layout_id,
            sizeof(out_snapshot->active_layout_id),
            service->active_layout->identity.layout_id,
            true);
        out_snapshot->active_layout_revision =
            service->active_layout->version.revision;
        out_snapshot->dirty =
            umi_workbench_layout_document_has_flag(
                service->active_layout,
                UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY);
    }
    /* Apply this operation only while the related capability or state is available. */
    if (service->has_active_session) {
        (void)umi_workbench_layout_copy_text(
            out_snapshot->active_session_id,
            sizeof(out_snapshot->active_session_id),
            service->active_session->session_id,
            true);
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_workbench_recovery_plan_build(
                service->recovery,
                service->active_session->session_id,
                &plan) == UMI_STATUS_OK) {
            out_snapshot->recovery_available =
                plan.recovery_required;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout service active layout operation used by this module and its
 * client applications.
 */
const UmiWorkbenchLayoutDocument *
umi_workbench_layout_service_active_layout(
    const UmiWorkbenchLayoutService *service)
{
    return service != NULL && service->has_active_layout
        ? service->active_layout
        : NULL;
}

/*
 * Provide the workbench layout service active layout mutable operation used by this module
 * and its client applications.
 */
UmiWorkbenchLayoutDocument *
umi_workbench_layout_service_active_layout_mutable(
    UmiWorkbenchLayoutService *service)
{
    return service != NULL && service->has_active_layout
        ? service->active_layout
        : NULL;
}

/*
 * Provide the workbench layout service active session operation used by this module and
 * its client applications.
 */
const UmiWorkbenchLayoutSession *
umi_workbench_layout_service_active_session(
    const UmiWorkbenchLayoutService *service)
{
    return service != NULL && service->has_active_session
        ? service->active_session
        : NULL;
}

/*
 * Provide the workbench layout service templates operation used by this module and its
 * client applications.
 */
UmiWorkbenchLayoutTemplateRegistry *
umi_workbench_layout_service_templates(
    UmiWorkbenchLayoutService *service)
{
    return service != NULL ? service->templates : NULL;
}

/*
 * Provide the workbench layout service panels operation used by this module and its client
 * applications.
 */
UmiWorkbenchPanelRegistry *
umi_workbench_layout_service_panels(
    UmiWorkbenchLayoutService *service)
{
    return service != NULL ? service->panels : NULL;
}

/*
 * Provide the workbench layout service perspectives operation used by this module and its
 * client applications.
 */
UmiWorkbenchPerspectiveRegistry *
umi_workbench_layout_service_perspectives(
    UmiWorkbenchLayoutService *service)
{
    return service != NULL ? service->perspectives : NULL;
}

/*
 * Provide the workbench layout service events operation used by this module and its client
 * applications.
 */
UmiWorkbenchLayoutEventBus *
umi_workbench_layout_service_events(
    UmiWorkbenchLayoutService *service)
{
    return service != NULL ? service->events : NULL;
}
