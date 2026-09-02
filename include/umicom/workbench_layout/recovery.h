/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/recovery.h
 *
 * PURPOSE:
 *   Journal layout and session checkpoints so the Framework can recover after a crash without persisting toolkit widget trees.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_RECOVERY_H
#define UMICOM_WORKBENCH_LAYOUT_RECOVERY_H

#include "umicom/workbench_layout/session.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named workbench recovery entry kind values accepted by this public contract.
 */
typedef enum UmiWorkbenchRecoveryEntryKind {
    UMI_WORKBENCH_RECOVERY_LAYOUT_CHECKPOINT = 1,
    UMI_WORKBENCH_RECOVERY_SESSION_CHECKPOINT = 2,
    UMI_WORKBENCH_RECOVERY_OPERATION_APPLIED = 3,
    UMI_WORKBENCH_RECOVERY_CLEAN_SHUTDOWN = 4,
    UMI_WORKBENCH_RECOVERY_FAILURE_MARKER = 5
} UmiWorkbenchRecoveryEntryKind;

/**
 * Represent the workbench recovery entry data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchRecoveryEntry {
    uint32_t structure_size;
    char entry_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiWorkbenchRecoveryEntryKind kind;
    char session_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char layout_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char correlation_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t layout_revision;
    uint64_t session_revision;
    uint64_t content_hash;
    bool durable;
} UmiWorkbenchRecoveryEntry;

/**
 * Represent the workbench recovery journal data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchRecoveryJournal {
    uint32_t structure_size;
    UmiWorkbenchRecoveryEntry
        entries[UMI_WORKBENCH_LAYOUT_MAX_RECOVERY_ENTRIES];
    size_t count;
    uint64_t next_sequence;
    uint64_t revision;
} UmiWorkbenchRecoveryJournal;

/**
 * Represent the workbench recovery plan data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchRecoveryPlan {
    uint32_t structure_size;
    bool recovery_required;
    char session_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char layout_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    uint64_t last_safe_sequence;
    uint64_t last_layout_revision;
    uint64_t last_session_revision;
    size_t unapplied_entry_count;
    char reason[UMI_WORKBENCH_LAYOUT_ERROR_CAPACITY];
} UmiWorkbenchRecoveryPlan;

/**
 * Initialise workbench recovery journal from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_recovery_journal_init(
    UmiWorkbenchRecoveryJournal *journal);

/**
 * Add workbench recovery journal only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_recovery_journal_append(
    UmiWorkbenchRecoveryJournal *journal,
    const UmiWorkbenchRecoveryEntry *entry);

/**
 * Provide the workbench recovery journal checkpoint layout operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_recovery_journal_checkpoint_layout(
    UmiWorkbenchRecoveryJournal *journal,
    const char *entry_id,
    const UmiWorkbenchLayoutDocument *document,
    const char *session_id,
    const char *correlation_id,
    uint64_t timestamp_ms);

/**
 * Provide the workbench recovery journal checkpoint session operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_recovery_journal_checkpoint_session(
    UmiWorkbenchRecoveryJournal *journal,
    const char *entry_id,
    const UmiWorkbenchLayoutSession *session,
    const char *layout_id,
    const char *correlation_id,
    uint64_t timestamp_ms);

/**
 * Provide the workbench recovery journal mark clean shutdown operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_recovery_journal_mark_clean_shutdown(
    UmiWorkbenchRecoveryJournal *journal,
    const char *entry_id,
    const char *session_id,
    uint64_t timestamp_ms);

/**
 * Provide the workbench recovery plan build operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_recovery_plan_build(
    const UmiWorkbenchRecoveryJournal *journal,
    const char *session_id,
    UmiWorkbenchRecoveryPlan *out_plan);

/**
 * Find workbench recovery journal while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchRecoveryEntry *
umi_workbench_recovery_journal_at(
    const UmiWorkbenchRecoveryJournal *journal,
    size_t index);

/**
 * Provide the workbench recovery entry kind text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_recovery_entry_kind_text(
    UmiWorkbenchRecoveryEntryKind kind);

#ifdef __cplusplus
}
#endif

#endif
