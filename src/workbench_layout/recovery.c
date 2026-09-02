/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/recovery.c
 *
 * PURPOSE:
 *   Implement bounded recovery journalling and crash-recovery planning for layout and session checkpoints without serialising toolkit widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/recovery.h"

#include <string.h>

#include "internal.h"

/* Provide the prepare entry operation used by this module and its client applications. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            entry->session_id,
            sizeof(entry->session_id),
            session_id,
            false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            entry->layout_id,
            sizeof(entry->layout_id),
            layout_id != NULL ? layout_id : "",
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            entry->correlation_id,
            sizeof(entry->correlation_id),
            correlation_id != NULL ? correlation_id : "",
            true);
    }
    return status;
}

/*
 * Initialise workbench recovery journal from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_recovery_journal_init(
    UmiWorkbenchRecoveryJournal *journal)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal == NULL) {
        return;
    }
    (void)memset(journal, 0, sizeof(*journal));
    journal->structure_size = sizeof(*journal);
    journal->next_sequence = 1U;
    journal->revision = 1U;
}

/*
 * Add workbench recovery journal only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_recovery_journal_append(
    UmiWorkbenchRecoveryJournal *journal,
    const UmiWorkbenchRecoveryEntry *entry)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal == NULL || entry == NULL ||
        entry->structure_size < sizeof(*entry) ||
        !umi_workbench_layout_text_present(entry->entry_id) ||
        entry->kind < UMI_WORKBENCH_RECOVERY_LAYOUT_CHECKPOINT ||
        entry->kind > UMI_WORKBENCH_RECOVERY_FAILURE_MARKER) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (journal->count >=
        UMI_WORKBENCH_LAYOUT_MAX_RECOVERY_ENTRIES) {
        /* Visit each bounded item once so every record receives the same rule. */
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (entry->sequence >= journal->next_sequence) {
        journal->next_sequence = entry->sequence + 1U;
    }
    journal->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench recovery journal checkpoint layout operation used by this module
 * and its client applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    entry.layout_revision = document->version.revision;
    entry.content_hash = document->content_hash;
    return umi_workbench_recovery_journal_append(
        journal, &entry);
}

/*
 * Provide the workbench recovery journal checkpoint session operation used by this module
 * and its client applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    entry.session_revision = session->revision;
    return umi_workbench_recovery_journal_append(
        journal, &entry);
}

/*
 * Provide the workbench recovery journal mark clean shutdown operation used by this module
 * and its client applications.
 */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_workbench_recovery_journal_append(
        journal, &entry);
}

/*
 * Provide the workbench recovery plan build operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_recovery_plan_build(
    const UmiWorkbenchRecoveryJournal *journal,
    const char *session_id,
    UmiWorkbenchRecoveryPlan *out_plan)
{
    const UmiWorkbenchRecoveryEntry *last_clean = NULL;
    const UmiWorkbenchRecoveryEntry *last_layout = NULL;
    const UmiWorkbenchRecoveryEntry *last_session = NULL;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < journal->count; ++index) {
        const UmiWorkbenchRecoveryEntry *entry =
            &journal->entries[index];

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(entry->session_id, session_id) != 0) {
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->kind ==
            UMI_WORKBENCH_RECOVERY_CLEAN_SHUTDOWN) {
            last_clean = entry;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (entry->kind ==
                   UMI_WORKBENCH_RECOVERY_LAYOUT_CHECKPOINT) {
            last_layout = entry;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (entry->kind ==
                   UMI_WORKBENCH_RECOVERY_SESSION_CHECKPOINT) {
            last_session = entry;
        }
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (last_session != NULL) {
        out_plan->last_session_revision =
            last_session->session_revision;
        /* Apply this branch only when its contract condition is satisfied. */
        if (last_session->sequence >
            out_plan->last_safe_sequence) {
            out_plan->last_safe_sequence =
                last_session->sequence;
        }
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        out_plan->recovery_required = false;
        (void)umi_workbench_layout_copy_text(
            out_plan->reason,
            sizeof(out_plan->reason),
            "No unclean workbench session requires recovery.",
            false);
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < journal->count; ++index) {
        const UmiWorkbenchRecoveryEntry *entry =
            &journal->entries[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(entry->session_id, session_id) == 0 &&
            entry->sequence > out_plan->last_safe_sequence &&
            entry->kind !=
                UMI_WORKBENCH_RECOVERY_CLEAN_SHUTDOWN) {
            out_plan->unapplied_entry_count += 1U;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Find workbench recovery journal while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchRecoveryEntry *
umi_workbench_recovery_journal_at(
    const UmiWorkbenchRecoveryJournal *journal,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal == NULL || index >= journal->count) {
        return NULL;
    }
    return &journal->entries[index];
}

/*
 * Provide the workbench recovery entry kind text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_recovery_entry_kind_text(
    UmiWorkbenchRecoveryEntryKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
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
