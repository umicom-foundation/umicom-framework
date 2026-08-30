/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/persistence.c
 *
 * PURPOSE:
 *   Implement authoritative layout and session persistence with validation, optimistic revisions, transactions and recovery checkpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/persistence.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

struct UmiWorkbenchLayoutPersistenceService {
    UmiWorkbenchLayoutStoreAdapter adapter;
    UmiWorkbenchLayoutPersistenceConfig config;
    UmiWorkbenchRecoveryJournal *recovery_journal;
    uint64_t checkpoint_sequence;
};

static void persistence_result_init(
    UmiWorkbenchLayoutPersistenceResult *result,
    UmiStatus status,
    const char *message)
{
    if (result == NULL) {
        return;
    }
    (void)memset(result, 0, sizeof(*result));
    result->structure_size = sizeof(*result);
    result->status = status;
    umi_workbench_layout_validation_report_init(
        &result->validation);
    (void)umi_workbench_layout_copy_text(
        result->message,
        sizeof(result->message),
        message != NULL ? message : "",
        true);
}

static UmiStatus update_audit(
    UmiWorkbenchLayoutPersistenceService *service,
    UmiWorkbenchLayoutDocument *document,
    const char *actor_id,
    const char *correlation_id,
    uint64_t timestamp_ms)
{
    UmiStatus status = UMI_STATUS_OK;

    if (!service->config.update_modified_audit) {
        return UMI_STATUS_OK;
    }
    if (!umi_workbench_layout_text_present(actor_id) ||
        timestamp_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_layout_copy_text(
        document->audit.modified_by,
        sizeof(document->audit.modified_by),
        actor_id,
        false);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            document->audit.correlation_id,
            sizeof(document->audit.correlation_id),
            correlation_id != NULL ? correlation_id : "",
            true);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }

    document->audit.modified_at_ms = timestamp_ms;
    if (document->audit.created_at_ms == 0U) {
        document->audit.created_at_ms = timestamp_ms;
        (void)umi_workbench_layout_copy_text(
            document->audit.created_by,
            sizeof(document->audit.created_by),
            actor_id,
            false);
    }
    return UMI_STATUS_OK;
}

static UmiStatus validate_document(
    UmiWorkbenchLayoutPersistenceService *service,
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutValidationReport *out_report)
{
    UmiWorkbenchLayoutValidationOptions options;

    if (!service->config.validate_before_save) {
        umi_workbench_layout_validation_report_init(out_report);
        out_report->valid = true;
        return UMI_STATUS_OK;
    }

    options =
        umi_workbench_layout_validation_options_default();
    options.validate_hash = true;
    return umi_workbench_layout_validate(
        document, &options, out_report);
}

static UmiStatus begin_if_supported(
    UmiWorkbenchLayoutPersistenceService *service,
    bool *out_started)
{
    *out_started = false;
    if (!umi_workbench_layout_store_supports_transactions(
            &service->adapter)) {
        return UMI_STATUS_OK;
    }

    {
        UmiStatus status =
            service->adapter.begin_transaction(
                service->adapter.context);
        if (status == UMI_STATUS_OK) {
            *out_started = true;
        }
        return status;
    }
}

static void rollback_if_started(
    UmiWorkbenchLayoutPersistenceService *service,
    bool started)
{
    if (started) {
        (void)service->adapter.rollback_transaction(
            service->adapter.context);
    }
}

static UmiStatus commit_if_started(
    UmiWorkbenchLayoutPersistenceService *service,
    bool started)
{
    return started
        ? service->adapter.commit_transaction(
              service->adapter.context)
        : UMI_STATUS_OK;
}

static UmiStatus checkpoint_layout_recovery(
    UmiWorkbenchLayoutPersistenceService *service,
    const UmiWorkbenchLayoutDocument *document,
    const char *session_id,
    const char *correlation_id,
    uint64_t timestamp_ms)
{
    char entry_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiStatus status;

    if (service->recovery_journal == NULL) {
        return UMI_STATUS_OK;
    }

    service->checkpoint_sequence += 1U;
    status = umi_workbench_layout_format(
        entry_id,
        sizeof(entry_id),
        "layout-checkpoint-%" PRIu64,
        service->checkpoint_sequence);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_workbench_recovery_journal_checkpoint_layout(
        service->recovery_journal,
        entry_id,
        document,
        session_id != NULL && session_id[0] != '\0'
            ? session_id
            : "layout-service",
        correlation_id,
        timestamp_ms);
}

static UmiStatus checkpoint_session_recovery(
    UmiWorkbenchLayoutPersistenceService *service,
    const UmiWorkbenchLayoutSession *session,
    const char *layout_id,
    const char *correlation_id,
    uint64_t timestamp_ms)
{
    char entry_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiStatus status;

    if (service->recovery_journal == NULL) {
        return UMI_STATUS_OK;
    }

    service->checkpoint_sequence += 1U;
    status = umi_workbench_layout_format(
        entry_id,
        sizeof(entry_id),
        "session-checkpoint-%" PRIu64,
        service->checkpoint_sequence);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_workbench_recovery_journal_checkpoint_session(
        service->recovery_journal,
        entry_id,
        session,
        layout_id,
        correlation_id,
        timestamp_ms);
}

UmiWorkbenchLayoutPersistenceConfig
umi_workbench_layout_persistence_config_default(void)
{
    UmiWorkbenchLayoutPersistenceConfig config;

    (void)memset(&config, 0, sizeof(config));
    config.structure_size = sizeof(config);
    config.validate_before_save = true;
    config.require_transactions_for_session_checkpoint = true;
    config.update_modified_audit = true;
    config.refresh_content_hash = true;
    return config;
}

UmiStatus umi_workbench_layout_persistence_create(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const UmiWorkbenchLayoutPersistenceConfig *config,
    UmiWorkbenchRecoveryJournal *recovery_journal,
    UmiWorkbenchLayoutPersistenceService **out_service)
{
    UmiWorkbenchLayoutPersistenceService *service;
    UmiStatus status;

    if (adapter == NULL || out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;
    status = umi_workbench_layout_store_adapter_validate(adapter);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    service = (UmiWorkbenchLayoutPersistenceService *)
        calloc(1U, sizeof(*service));
    if (service == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    service->adapter = *adapter;
    service->config = config != NULL
        ? *config
        : umi_workbench_layout_persistence_config_default();
    service->recovery_journal = recovery_journal;
    service->checkpoint_sequence = 0U;
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_workbench_layout_persistence_destroy(
    UmiWorkbenchLayoutPersistenceService *service)
{
    free(service);
}

UmiStatus umi_workbench_layout_persistence_save_layout(
    UmiWorkbenchLayoutPersistenceService *service,
    UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision,
    const char *actor_id,
    const char *correlation_id,
    uint64_t timestamp_ms,
    UmiWorkbenchLayoutPersistenceResult *out_result)
{
    UmiWorkbenchLayoutDocument *working_document;
    UmiWorkbenchLayoutValidationReport validation;
    uint64_t previous_revision;
    uint64_t stored_revision = 0U;
    bool transaction_started = false;
    UmiStatus status;

    if (service == NULL || document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_workbench_layout_validation_report_init(&validation);
    previous_revision = document->version.revision;
    working_document =
        (UmiWorkbenchLayoutDocument *)malloc(
            sizeof(*working_document));
    if (working_document == NULL) {
        persistence_result_init(
            out_result,
            UMI_STATUS_OUT_OF_MEMORY,
            "Layout persistence could not allocate a working snapshot.");
        if (out_result != NULL) {
            out_result->previous_revision = previous_revision;
        }
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    *working_document = *document;
    working_document->flags &=
        ~(uint32_t)UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY;

    status = update_audit(
        service,
        working_document,
        actor_id,
        correlation_id,
        timestamp_ms);
    if (status == UMI_STATUS_OK &&
        service->config.refresh_content_hash) {
        umi_workbench_layout_document_refresh_hash(
            working_document);
    }
    if (status == UMI_STATUS_OK) {
        status = validate_document(
            service, working_document, &validation);
    }
    if (status == UMI_STATUS_OK && !validation.valid) {
        status = UMI_STATUS_INVALID_STATE;
    }
    if (status != UMI_STATUS_OK) {
        persistence_result_init(
            out_result,
            status,
            "Layout validation failed before persistence.");
        if (out_result != NULL) {
            out_result->previous_revision = previous_revision;
            out_result->validation = validation;
        }
        free(working_document);
        return status;
    }

    status = begin_if_supported(
        service, &transaction_started);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_store_save(
            &service->adapter,
            working_document,
            expected_revision,
            &stored_revision);
    }
    if (status == UMI_STATUS_OK) {
        working_document->version.revision =
            stored_revision;
        if (service->config.refresh_content_hash) {
            umi_workbench_layout_document_refresh_hash(
                working_document);
        }
        status = checkpoint_layout_recovery(
            service,
            working_document,
            "layout-service",
            correlation_id,
            timestamp_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = commit_if_started(
            service, transaction_started);
        if (status != UMI_STATUS_OK) {
            rollback_if_started(
                service, transaction_started);
        }
    } else {
        rollback_if_started(
            service, transaction_started);
    }

    if (status == UMI_STATUS_OK) {
        *document = *working_document;
    }

    persistence_result_init(
        out_result,
        status,
        status == UMI_STATUS_OK
            ? "The semantic layout was persisted."
            : "The semantic layout could not be persisted.");
    if (out_result != NULL) {
        out_result->previous_revision = previous_revision;
        out_result->resulting_revision =
            status == UMI_STATUS_OK
                ? stored_revision
                : previous_revision;
        out_result->validation = validation;
        out_result->transaction_used =
            transaction_started;
        out_result->recovery_checkpoint_written =
            status == UMI_STATUS_OK &&
            service->recovery_journal != NULL;
    }

    free(working_document);
    return status;
}

UmiStatus umi_workbench_layout_persistence_load_layout(
    UmiWorkbenchLayoutPersistenceService *service,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document,
    UmiWorkbenchLayoutPersistenceResult *out_result)
{
    UmiWorkbenchLayoutValidationReport validation;
    UmiStatus status;

    if (service == NULL || out_document == NULL ||
        !umi_workbench_layout_text_present(layout_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_workbench_layout_validation_report_init(&validation);
    status = umi_workbench_layout_store_load(
        &service->adapter, layout_id, out_document);
    if (status == UMI_STATUS_OK) {
        status = validate_document(
            service, out_document, &validation);
    }
    if (status == UMI_STATUS_OK && !validation.valid) {
        status = UMI_STATUS_INVALID_STATE;
    }

    persistence_result_init(
        out_result,
        status,
        status == UMI_STATUS_OK
            ? "The semantic layout was loaded."
            : "The semantic layout could not be loaded.");
    if (out_result != NULL) {
        out_result->resulting_revision =
            status == UMI_STATUS_OK
                ? out_document->version.revision
                : 0U;
        out_result->validation = validation;
    }
    return status;
}

UmiStatus umi_workbench_layout_persistence_delete_layout(
    UmiWorkbenchLayoutPersistenceService *service,
    const char *layout_id,
    uint64_t expected_revision,
    UmiWorkbenchLayoutPersistenceResult *out_result)
{
    UmiStatus status;

    if (service == NULL ||
        !umi_workbench_layout_text_present(layout_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_store_delete(
        &service->adapter, layout_id, expected_revision);
    persistence_result_init(
        out_result,
        status,
        status == UMI_STATUS_OK
            ? "The semantic layout was deleted."
            : "The semantic layout could not be deleted.");
    if (out_result != NULL) {
        out_result->previous_revision = expected_revision;
    }
    return status;
}

UmiStatus umi_workbench_layout_persistence_checkpoint_session(
    UmiWorkbenchLayoutPersistenceService *service,
    UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutSession *session,
    const char *actor_id,
    const char *correlation_id,
    uint64_t timestamp_ms,
    UmiWorkbenchLayoutPersistenceResult *out_result)
{
    UmiWorkbenchLayoutDocument *working_document;
    UmiWorkbenchLayoutSession working_session;
    UmiWorkbenchLayoutValidationReport validation;
    uint64_t layout_previous;
    uint64_t layout_revision = 0U;
    uint64_t session_previous;
    uint64_t session_revision = 0U;
    bool transaction_started = false;
    UmiStatus status;

    if (service == NULL || document == NULL ||
        session == NULL || timestamp_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (service->config.require_transactions_for_session_checkpoint &&
        !umi_workbench_layout_store_supports_transactions(
            &service->adapter)) {
        return UMI_STATUS_UNAVAILABLE;
    }

    umi_workbench_layout_validation_report_init(&validation);
    layout_previous = document->version.revision;
    session_previous = session->revision;
    working_document =
        (UmiWorkbenchLayoutDocument *)malloc(
            sizeof(*working_document));
    if (working_document == NULL) {
        persistence_result_init(
            out_result,
            UMI_STATUS_OUT_OF_MEMORY,
            "The workbench checkpoint could not allocate a working snapshot.");
        if (out_result != NULL) {
            out_result->previous_revision = layout_previous;
        }
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    *working_document = *document;
    working_session = *session;
    working_document->flags &=
        ~(uint32_t)UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY;

    status = update_audit(
        service,
        working_document,
        actor_id,
        correlation_id,
        timestamp_ms);
    if (status == UMI_STATUS_OK &&
        service->config.refresh_content_hash) {
        umi_workbench_layout_document_refresh_hash(
            working_document);
    }
    if (status == UMI_STATUS_OK) {
        status = validate_document(
            service, working_document, &validation);
    }
    if (status == UMI_STATUS_OK && !validation.valid) {
        status = UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_session_validate(
            &working_session);
    }
    if (status == UMI_STATUS_OK) {
        status = begin_if_supported(
            service, &transaction_started);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_store_save(
            &service->adapter,
            working_document,
            layout_previous,
            &layout_revision);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_store_save_session(
            &service->adapter,
            &working_session,
            session_previous,
            &session_revision);
        if (status == UMI_STATUS_NOT_FOUND) {
            status = umi_workbench_layout_store_save_session(
                &service->adapter,
                &working_session,
                0U,
                &session_revision);
        }
    }
    if (status == UMI_STATUS_OK) {
        working_document->version.revision = layout_revision;
        working_session.revision = session_revision;
        status = umi_workbench_layout_session_mark_checkpoint(
            &working_session, timestamp_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = checkpoint_layout_recovery(
            service,
            working_document,
            working_session.session_id,
            correlation_id,
            timestamp_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = checkpoint_session_recovery(
            service,
            &working_session,
            working_document->identity.layout_id,
            correlation_id,
            timestamp_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = commit_if_started(
            service, transaction_started);
        if (status != UMI_STATUS_OK) {
            rollback_if_started(
                service, transaction_started);
        }
    } else {
        rollback_if_started(service, transaction_started);
    }

    if (status == UMI_STATUS_OK) {
        *document = *working_document;
        *session = working_session;
    }

    persistence_result_init(
        out_result,
        status,
        status == UMI_STATUS_OK
            ? "The workbench layout and session checkpoint committed."
            : "The workbench checkpoint was rolled back.");
    if (out_result != NULL) {
        out_result->previous_revision = layout_previous;
        out_result->resulting_revision =
            status == UMI_STATUS_OK
                ? layout_revision
                : layout_previous;
        out_result->validation = validation;
        out_result->transaction_used =
            transaction_started;
        out_result->recovery_checkpoint_written =
            status == UMI_STATUS_OK &&
            service->recovery_journal != NULL;
    }

    free(working_document);
    return status;
}

UmiStatus umi_workbench_layout_persistence_restore_session(
    UmiWorkbenchLayoutPersistenceService *service,
    const char *session_id,
    UmiWorkbenchLayoutSession *out_session,
    UmiWorkbenchLayoutDocument *out_document,
    UmiWorkbenchLayoutPersistenceResult *out_result)
{
    UmiStatus status;

    if (service == NULL || out_session == NULL ||
        out_document == NULL ||
        !umi_workbench_layout_text_present(session_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_layout_store_load_session(
        &service->adapter, session_id, out_session);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_store_load(
            &service->adapter,
            out_session->active_layout_id,
            out_document);
    }

    persistence_result_init(
        out_result,
        status,
        status == UMI_STATUS_OK
            ? "The workbench session and active layout were restored."
            : "The workbench session could not be restored.");
    if (out_result != NULL && status == UMI_STATUS_OK) {
        out_result->resulting_revision =
            out_document->version.revision;
    }
    return status;
}

const UmiWorkbenchLayoutStoreAdapter *
umi_workbench_layout_persistence_adapter(
    const UmiWorkbenchLayoutPersistenceService *service)
{
    return service != NULL ? &service->adapter : NULL;
}
