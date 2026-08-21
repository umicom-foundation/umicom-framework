/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/recovery.c
 *
 * PURPOSE:
 *   Implement bounded recovery journalling and crash-recovery planning for layout and session checkpoints without serialising toolkit widgets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/recovery.h"

#include <string.h>

#include "internal.h"

static UmiStatus prepare_entry(
    UmiWorkbenchRecoveryJournal *journal,
    UmiWorkbenchRecoveryEntry *entry,
    const char *entry_id,
    UmiWorkbenchRecoveryEntryKind kind,
    const char *session_id,
    const char *layout_id,
    const char *correlation_id,
    uint64_t timestamp_ms)
{
    UmiStatus status;

    if (journal == NULL || entry == NULL ||
        !umi_workbench_layout_text_present(entry_id) ||
        !umi_workbench_layout_text_present(session_id) ||
        timestamp_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(entry, 0, sizeof(*entry));
    entry->structure_size = sizeof(*entry);
    entry->kind = kind;
    entry->sequence = journal->next_sequence;
    entry->timestamp_ms = timestamp_ms;
    entry->durable = true;

    status = umi_workbench_layout_copy_text(
        entry->entry_id,
        sizeof(entry->entry_id),
        entry_id,
        false);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            entry->session_id,
            sizeof(entry->session_id),
            session_id,
            false);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            entry->layout_id,
            sizeof(entry->layout_id),
            layout_id != NULL ? layout_id : "",
            true);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            entry->correlation_id,
            sizeof(entry->correlation_id),
            correlation_id != NULL ? correlation_id : "",
            true);
    }
    return status;
}

void umi_workbench_recovery_journal_init(
    UmiWorkbenchRecoveryJournal *journal)
{
    if (journal == NULL) {
        return;
    }
    (void)memset(journal, 0, sizeof(*journal));
    journal->structure_size = sizeof(*journal);
    journal->next_sequence = 1U;
    journal->revision = 1U;
}

UmiStatus umi_workbench_recovery_journal_append(
    UmiWorkbenchRecoveryJournal *journal,
    const UmiWorkbenchRecoveryEntry *entry)
{
    size_t index;

    if (journal == NULL || entry == NULL ||
        entry->structure_size < sizeof(*entry) ||
        !umi_workbench_layout_text_present(entry->entry_id) ||
        entry->kind < UMI_WORKBENCH_RECOVERY_LAYOUT_CHECKPOINT ||
        entry->kind > UMI_WORKBENCH_RECOVERY_FAILURE_MARKER) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (journal->count >=
        UMI_WORKBENCH_LAYOUT_MAX_RECOVERY_ENTRIES) {
        for (index = 1U; index < journal->count; ++index) {
            journal->entries[index - 1U] =
                journal->entries[index];
        }
        journal->count -= 1U;
    }

    journal->entries[journal->count] = *entry;
    journal->entries[journal->count].structure_size =
        sizeof(journal->entries[journal->count]);
    journal->count += 1U;
    if (entry->sequence >= journal->next_sequence) {
        journal->next_sequence = entry->sequence + 1U;
    }
    journal->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_recovery_journal_checkpoint_layout(
    UmiWorkbenchRecoveryJournal *journal,
    const char *entry_id,
    const UmiWorkbenchLayoutDocument *document,
    const char *session_id,
    const char *correlation_id,
    uint64_t timestamp_ms)
{
    UmiWorkbenchRecoveryEntry entry;
    UmiStatus status;

    if (document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = prepare_entry(
        journal,
        &entry,
        entry_id,
        UMI_WORKBENCH_RECOVERY_LAYOUT_CHECKPOINT,
        session_id,
        document->identity.layout_id,
        correlation_id,
        timestamp_ms);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    entry.layout_revision = document->version.revision;
    entry.content_hash = document->content_hash;
    return umi_workbench_recovery_journal_append(
        journal, &entry);
}

UmiStatus umi_workbench_recovery_journal_checkpoint_session(
    UmiWorkbenchRecoveryJournal *journal,
    const char *entry_id,
    const UmiWorkbenchLayoutSession *session,
    const char *layout_id,
    const char *correlation_id,
    uint64_t timestamp_ms)
{
    UmiWorkbenchRecoveryEntry entry;
    UmiStatus status;

    if (session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = prepare_entry(
        journal,
        &entry,
        entry_id,
        UMI_WORKBENCH_RECOVERY_SESSION_CHECKPOINT,
        session->session_id,
        layout_id != NULL
            ? layout_id
            : session->active_layout_id,
        correlation_id,
        timestamp_ms);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    entry.session_revision = session->revision;
    return umi_workbench_recovery_journal_append(
        journal, &entry);
}

UmiStatus umi_workbench_recovery_journal_mark_clean_shutdown(
    UmiWorkbenchRecoveryJournal *journal,
    const char *entry_id,
    const char *session_id,
    uint64_t timestamp_ms)
{
    UmiWorkbenchRecoveryEntry entry;
    UmiStatus status;

    status = prepare_entry(
        journal,
        &entry,
        entry_id,
        UMI_WORKBENCH_RECOVERY_CLEAN_SHUTDOWN,
        session_id,
        "",
        "",
        timestamp_ms);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_workbench_recovery_journal_append(
        journal, &entry);
}

UmiStatus umi_workbench_recovery_plan_build(
    const UmiWorkbenchRecoveryJournal *journal,
    const char *session_id,
    UmiWorkbenchRecoveryPlan *out_plan)
{
    const UmiWorkbenchRecoveryEntry *last_clean = NULL;
    const UmiWorkbenchRecoveryEntry *last_layout = NULL;
    const UmiWorkbenchRecoveryEntry *last_session = NULL;
    size_t index;

    if (journal == NULL || out_plan == NULL ||
        !umi_workbench_layout_text_present(session_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->structure_size = sizeof(*out_plan);
    (void)umi_workbench_layout_copy_text(
        out_plan->session_id,
        sizeof(out_plan->session_id),
        session_id,
        false);

    for (index = 0U; index < journal->count; ++index) {
        const UmiWorkbenchRecoveryEntry *entry =
            &journal->entries[index];

        if (strcmp(entry->session_id, session_id) != 0) {
            continue;
        }
        if (entry->kind ==
            UMI_WORKBENCH_RECOVERY_CLEAN_SHUTDOWN) {
            last_clean = entry;
        } else if (entry->kind ==
                   UMI_WORKBENCH_RECOVERY_LAYOUT_CHECKPOINT) {
            last_layout = entry;
        } else if (entry->kind ==
                   UMI_WORKBENCH_RECOVERY_SESSION_CHECKPOINT) {
            last_session = entry;
        }
    }

    if (last_layout != NULL) {
        (void)umi_workbench_layout_copy_text(
            out_plan->layout_id,
            sizeof(out_plan->layout_id),
            last_layout->layout_id,
            true);
        out_plan->last_layout_revision =
            last_layout->layout_revision;
        out_plan->last_safe_sequence =
            last_layout->sequence;
    }
    if (last_session != NULL) {
        out_plan->last_session_revision =
            last_session->session_revision;
        if (last_session->sequence >
            out_plan->last_safe_sequence) {
            out_plan->last_safe_sequence =
                last_session->sequence;
        }
    }

    if ((last_layout != NULL || last_session != NULL) &&
        (last_clean == NULL ||
         last_clean->sequence <
             out_plan->last_safe_sequence)) {
        out_plan->recovery_required = true;
        (void)umi_workbench_layout_copy_text(
            out_plan->reason,
            sizeof(out_plan->reason),
            "A durable checkpoint exists after the last clean shutdown marker.",
            false);
    } else {
        out_plan->recovery_required = false;
        (void)umi_workbench_layout_copy_text(
            out_plan->reason,
            sizeof(out_plan->reason),
            "No unclean workbench session requires recovery.",
            false);
    }

    for (index = 0U; index < journal->count; ++index) {
        const UmiWorkbenchRecoveryEntry *entry =
            &journal->entries[index];
        if (strcmp(entry->session_id, session_id) == 0 &&
            entry->sequence > out_plan->last_safe_sequence &&
            entry->kind !=
                UMI_WORKBENCH_RECOVERY_CLEAN_SHUTDOWN) {
            out_plan->unapplied_entry_count += 1U;
        }
    }
    return UMI_STATUS_OK;
}

const UmiWorkbenchRecoveryEntry *
umi_workbench_recovery_journal_at(
    const UmiWorkbenchRecoveryJournal *journal,
    size_t index)
{
    if (journal == NULL || index >= journal->count) {
        return NULL;
    }
    return &journal->entries[index];
}

const char *umi_workbench_recovery_entry_kind_text(
    UmiWorkbenchRecoveryEntryKind kind)
{
    switch (kind) {
    case UMI_WORKBENCH_RECOVERY_LAYOUT_CHECKPOINT:
        return "layout-checkpoint";
    case UMI_WORKBENCH_RECOVERY_SESSION_CHECKPOINT:
        return "session-checkpoint";
    case UMI_WORKBENCH_RECOVERY_OPERATION_APPLIED:
        return "operation-applied";
    case UMI_WORKBENCH_RECOVERY_CLEAN_SHUTDOWN:
        return "clean-shutdown";
    case UMI_WORKBENCH_RECOVERY_FAILURE_MARKER:
        return "failure-marker";
    default:
        return "invalid";
    }
}
