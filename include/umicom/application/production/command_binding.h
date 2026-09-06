/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/command_binding.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_COMMAND_BINDING_H
#define UMICOM_APPLICATION_PRODUCTION_COMMAND_BINDING_H

#include <stdbool.h>
#include <stdint.h>

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/application_binding.h"
#include "umicom/application/runtime/command_surface.h"

/**
 * Represent the application production command binding data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionCommandBinding {
    const UmiApplicationCommandDescriptor *command;
    int enabled;
    int mutates_workspace;
} UmiApplicationProductionCommandBinding;

/**
 * Represent the application production command bindings data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionCommandBindings {
    UmiApplicationCommandSurface surface;
    UmiApplicationProductionCommandBinding
        entries[UMI_APPLICATION_PRODUCTION_MAX_COMMANDS];
    size_t count;
    size_t enabled_count;
} UmiApplicationProductionCommandBindings;

/* Retain enough recent actions for an activity panel without making runtimes unbounded. */
#define UMI_APPLICATION_PRODUCTION_COMMAND_JOURNAL_CAPACITY 64U

/**
 * Keep one bounded record for every resolved command invocation. The copied target ID means
 * the history remains readable even when a caller releases its temporary request text.
 */
typedef struct UmiApplicationProductionCommandJournalEntry {
    uint64_t sequence;
    UmiApplicationCommandKind kind;
    char target_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    bool available;
    bool approval_requested;
    bool approved;
    bool executed;
    UmiStatus status;
} UmiApplicationProductionCommandJournalEntry;

/**
 * Store recent command outcomes in a fixed-size, newest-first-safe history without heap
 * allocation. When full, the oldest entry is dropped so the latest failure remains visible.
 */
typedef struct UmiApplicationProductionCommandJournal {
    UmiApplicationProductionCommandJournalEntry
        entries[UMI_APPLICATION_PRODUCTION_COMMAND_JOURNAL_CAPACITY];
    size_t count;
    uint64_t next_sequence;
} UmiApplicationProductionCommandJournal;

/**
 * Let a product ask for confirmation before a command that changes its workspace is run.
 * The callback belongs to the product UI, while the Framework still owns all safety checks.
 */
typedef UmiStatus (*UmiApplicationProductionCommandApproval)(
    const UmiApplicationCommandDescriptor *command,
    void *user_data);

/**
 * Let a product perform the already-validated command action. The Framework passes only a
 * catalogue descriptor, so products can keep their own state and platform code private.
 */
typedef UmiStatus (*UmiApplicationProductionCommandExecutor)(
    const UmiApplicationCommandDescriptor *command,
    void *user_data);

/**
 * Describe one command invocation request without allocating memory or copying catalogue
 * strings. A caller can use this same request for a dry-run or a real UI action.
 */
typedef struct UmiApplicationProductionCommandInvocationRequest {
    uint32_t structure_size;
    UmiApplicationCommandKind kind;
    const char *target_id;
    UmiApplicationCapabilityProbe capability_probe;
    void *capability_probe_data;
    bool require_approval;
    UmiApplicationProductionCommandApproval approval;
    void *approval_data;
    UmiApplicationProductionCommandExecutor execute;
    void *execute_data;
    UmiApplicationProductionCommandJournal *journal;
} UmiApplicationProductionCommandInvocationRequest;

/**
 * Report each gate reached by a command invocation so a frontend can explain a blocked or
 * successful action without inspecting Framework internals.
 */
typedef struct UmiApplicationProductionCommandInvocationResult {
    uint32_t structure_size;
    const UmiApplicationProductionCommandBinding *binding;
    bool available;
    bool approval_requested;
    bool approved;
    bool executed;
    UmiStatus status;
} UmiApplicationProductionCommandInvocationResult;

/**
 * Initialise a command journal so its sequence starts at one and its history is empty.
 */
void umi_application_production_command_journal_init(
    UmiApplicationProductionCommandJournal *journal);

/**
 * Check the bounded journal before it is copied into a session checkpoint or restored.
 */
UmiStatus umi_application_production_command_journal_validate(
    const UmiApplicationProductionCommandJournal *journal);

/**
 * Capture a value-only command journal checkpoint without sharing mutable storage.
 */
UmiStatus umi_application_production_command_journal_capture(
    const UmiApplicationProductionCommandJournal *journal,
    UmiApplicationProductionCommandJournal *out_checkpoint);

/**
 * Restore a previously captured journal only after every count and sequence value is valid.
 */
UmiStatus umi_application_production_command_journal_restore(
    const UmiApplicationProductionCommandJournal *checkpoint,
    UmiApplicationProductionCommandJournal *out_journal);

/**
 * Find one retained command outcome by insertion order, where index zero is the oldest
 * retained entry and the final index is the newest entry.
 */
const UmiApplicationProductionCommandJournalEntry *
umi_application_production_command_journal_at(
    const UmiApplicationProductionCommandJournal *journal,
    size_t index);

/**
 * Provide the application production command bindings build operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_command_bindings_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionCommandBindings *out_bindings);
/**
 * Find application production command bindings while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationProductionCommandBinding *
umi_application_production_command_bindings_find(
    const UmiApplicationProductionCommandBindings *bindings,
    UmiApplicationCommandKind kind, const char *target_id);

/**
 * Resolve, gate and optionally execute a command selected from a production command
 * binding. The caller supplies approval and execution callbacks so no product behaviour is
 * hidden inside the Framework catalogue.
 */
UmiStatus umi_application_production_command_bindings_invoke(
    const UmiApplicationProductionCommandBindings *bindings,
    const UmiApplicationProductionCommandInvocationRequest *request,
    UmiApplicationProductionCommandInvocationResult *out_result);

#ifdef __cplusplus
}
#endif
#endif
