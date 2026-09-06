/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/command_binding.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/command_binding.h"

#include <string.h>

#include "umicom/base/text.h"

/* Record one resolved invocation so products can show a bounded action history. */
static UmiStatus record_invocation(
    UmiApplicationProductionCommandJournal *journal,
    const UmiApplicationProductionCommandInvocationResult *result)
{
    UmiApplicationProductionCommandJournalEntry *entry;
    char target_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    size_t index;
    UmiStatus status;

    /* A journal is optional, but a supplied result must always describe a command. */
    if (journal == NULL) return UMI_STATUS_OK;
    if (result == NULL || result->binding == NULL || result->binding->command == NULL ||
        result->binding->command->target_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Refuse a corrupted count before the ring-buffer move could address outside storage. */
    if (journal->count > UMI_APPLICATION_PRODUCTION_COMMAND_JOURNAL_CAPACITY ||
        journal->next_sequence == 0U)
        return UMI_STATUS_INVALID_STATE;
    /* Keep the sequence monotonic so consumers can correlate history with UI events. */
    if (journal->next_sequence == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Copy before rotating the ring so a rejected ID cannot discard an older record. */
    status = umi_text_copy(target_id, sizeof(target_id),
                           result->binding->command->target_id);
    if (status != UMI_STATUS_OK) return status;
    if (journal->count >= UMI_APPLICATION_PRODUCTION_COMMAND_JOURNAL_CAPACITY) {
        (void)memmove(&journal->entries[0], &journal->entries[1],
                      (journal->count - 1U) * sizeof(journal->entries[0]));
        journal->count -= 1U;
    }
    index = journal->count;
    entry = &journal->entries[index];
    (void)memcpy(entry->target_id, target_id, sizeof(entry->target_id));
    entry->sequence = journal->next_sequence++;
    entry->kind = result->binding->command->kind;
    entry->available = result->available;
    entry->approval_requested = result->approval_requested;
    entry->approved = result->approved;
    entry->executed = result->executed;
    entry->status = result->status;
    journal->count += 1U;
    return UMI_STATUS_OK;
}

/* Initialise the bounded command journal before any product action is recorded. */
void umi_application_production_command_journal_init(
    UmiApplicationProductionCommandJournal *journal)
{
    /* Clear stale history so a new session cannot display another session's actions. */
    if (journal == NULL) return;
    (void)memset(journal, 0, sizeof(*journal));
    journal->next_sequence = 1U;
}

/* Validate journal bounds and sequence state before a copy or restore can publish it. */
UmiStatus umi_application_production_command_journal_validate(
    const UmiApplicationProductionCommandJournal *journal)
{
    size_t index;
    uint64_t previous_sequence = 0U;

    /* A missing journal cannot safely be used as session evidence. */
    if (journal == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (journal->count > UMI_APPLICATION_PRODUCTION_COMMAND_JOURNAL_CAPACITY ||
        journal->next_sequence == 0U)
        return UMI_STATUS_INVALID_STATE;
    /* Every retained ID must be terminated so later UI code can display it safely. */
    for (index = 0U; index < journal->count; ++index) {
        if (memchr(journal->entries[index].target_id, '\0',
                   sizeof(journal->entries[index].target_id)) == NULL ||
            journal->entries[index].sequence == 0U ||
            (index > 0U && journal->entries[index].sequence <= previous_sequence) ||
            journal->entries[index].kind < UMI_APPLICATION_COMMAND_ACTIVATE_PANEL ||
            journal->entries[index].kind > UMI_APPLICATION_COMMAND_ACTIVATE_FEATURE ||
            journal->entries[index].status < UMI_STATUS_OK ||
            journal->entries[index].status > UMI_STATUS_BUSY)
            return UMI_STATUS_INVALID_STATE;
        previous_sequence = journal->entries[index].sequence;
    }
    /* The next sequence must remain ahead of the newest retained record. */
    if (journal->count > 0U && journal->next_sequence <= previous_sequence)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* Copy a validated journal into caller-owned checkpoint storage. */
UmiStatus umi_application_production_command_journal_capture(
    const UmiApplicationProductionCommandJournal *journal,
    UmiApplicationProductionCommandJournal *out_checkpoint)
{
    UmiStatus status;

    /* Validate before copying so a malformed source cannot become a checkpoint. */
    if (journal == NULL || out_checkpoint == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_command_journal_validate(journal);
    if (status != UMI_STATUS_OK) return status;
    (void)memcpy(out_checkpoint, journal, sizeof(*out_checkpoint));
    return UMI_STATUS_OK;
}

/* Restore a complete journal atomically after validation has passed. */
UmiStatus umi_application_production_command_journal_restore(
    const UmiApplicationProductionCommandJournal *checkpoint,
    UmiApplicationProductionCommandJournal *out_journal)
{
    UmiStatus status;

    /* Validate the complete checkpoint before changing the destination history. */
    if (checkpoint == NULL || out_journal == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_command_journal_validate(checkpoint);
    if (status != UMI_STATUS_OK) return status;
    (void)memcpy(out_journal, checkpoint, sizeof(*out_journal));
    return UMI_STATUS_OK;
}

/* Return one journal item without exposing storage ownership to callers. */
const UmiApplicationProductionCommandJournalEntry *
umi_application_production_command_journal_at(
    const UmiApplicationProductionCommandJournal *journal,
    size_t index)
{
    return journal != NULL &&
        journal->count <= UMI_APPLICATION_PRODUCTION_COMMAND_JOURNAL_CAPACITY &&
        index < journal->count
        ? &journal->entries[index] : NULL;
}

/* Finish an invocation and retain its outcome without replacing the action's status. */
static UmiStatus finish_invocation(
    const UmiApplicationProductionCommandInvocationRequest *request,
    UmiApplicationProductionCommandInvocationResult *result,
    UmiStatus status)
{
    result->status = status;
    /* Journal failure must not hide the product executor's real result. */
    if (request->journal != NULL) {
        (void)record_invocation(request->journal, result);
    }
    return status;
}

/*
 * Provide the application production command bindings build operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_command_bindings_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionCommandBindings *out_bindings)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_bindings == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_binding_validate(binding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_bindings, 0, sizeof(*out_bindings));
    status = umi_application_command_surface_build(
        binding->experience, &out_bindings->surface);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (out_bindings->surface.command_count >
        UMI_APPLICATION_PRODUCTION_MAX_COMMANDS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < out_bindings->surface.command_count; ++index) {
        UmiApplicationProductionCommandBinding *entry =
            &out_bindings->entries[index];
        entry->command = &out_bindings->surface.commands[index];
        entry->enabled =
            entry->command->kind != UMI_APPLICATION_COMMAND_ACTIVATE_FEATURE ||
            entry->command->feature_state != UMI_EXPERIENCE_FEATURE_PLANNED;
        entry->mutates_workspace =
            entry->command->kind != UMI_APPLICATION_COMMAND_ACTIVATE_FEATURE;
        out_bindings->enabled_count += (size_t)entry->enabled;
        out_bindings->count += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Find application production command bindings while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationProductionCommandBinding *
umi_application_production_command_bindings_find(
    const UmiApplicationProductionCommandBindings *bindings,
    UmiApplicationCommandKind kind, const char *target_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || target_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < bindings->count; ++index) {
        const UmiApplicationCommandDescriptor *command =
            bindings->entries[index].command;
        /* Use the shared build helper when it is available from the parent composition. */
        if (command != NULL && command->kind == kind &&
            strcmp(command->target_id, target_id) == 0)
            return &bindings->entries[index];
    }
    return NULL;
}

/*
 * Resolve, gate and optionally execute one palette selection through the production binding
 * layer. Product code supplies the final action callback so this function remains safe and
 * frontend independent.
 */
UmiStatus umi_application_production_command_bindings_invoke(
    const UmiApplicationProductionCommandBindings *bindings,
    const UmiApplicationProductionCommandInvocationRequest *request,
    UmiApplicationProductionCommandInvocationResult *out_result)
{
    const UmiApplicationProductionCommandBinding *binding;
    UmiStatus status;

    /* Validate public structures before reading their bounded arrays or callbacks. */
    if (bindings == NULL || request == NULL || out_result == NULL ||
        request->structure_size != sizeof(*request) ||
        bindings->surface.structure_size != sizeof(bindings->surface) ||
        bindings->count > UMI_APPLICATION_PRODUCTION_MAX_COMMANDS ||
        bindings->surface.command_count > UMI_APPLICATION_RUNTIME_MAX_COMMANDS ||
        request->kind < UMI_APPLICATION_COMMAND_ACTIVATE_PANEL ||
        request->kind > UMI_APPLICATION_COMMAND_ACTIVATE_FEATURE ||
        request->target_id == NULL || request->target_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->structure_size = (uint32_t)sizeof(*out_result);
    binding = umi_application_production_command_bindings_find(
        bindings, request->kind, request->target_id);
    /* A stale palette entry must not be sent to a product callback. */
    if (binding == NULL || binding->command == NULL) {
        return finish_invocation(request, out_result, UMI_STATUS_NOT_FOUND);
    }
    out_result->binding = binding;
    /* Disabled bindings are kept visible so the UI can explain why they cannot run. */
    if (binding->enabled == 0) {
        return finish_invocation(request, out_result, UMI_STATUS_UNAVAILABLE);
    }
    out_result->available = umi_application_command_descriptor_is_available(
        binding->command, request->capability_probe, request->capability_probe_data);
    /* Capability checks run before asking the user to approve or execute an action. */
    if (!out_result->available) {
        return finish_invocation(request, out_result, UMI_STATUS_UNAVAILABLE);
    }
    out_result->approval_requested = request->require_approval;
    if (request->require_approval) {
        /* A mutating action cannot proceed silently when approval was requested. */
        if (request->approval == NULL) {
            return finish_invocation(request, out_result,
                                     UMI_STATUS_PERMISSION_DENIED);
        }
        status = request->approval(binding->command, request->approval_data);
        /* Preserve the callback result so the caller receives the precise decision. */
        if (status != UMI_STATUS_OK) {
            return finish_invocation(request, out_result, status);
        }
    }
    out_result->approved = true;
    /* Without an executor this API still provides a safe resolution dry-run. */
    if (request->execute == NULL) {
        return finish_invocation(request, out_result, UMI_STATUS_NOT_IMPLEMENTED);
    }
    status = request->execute(binding->command, request->execute_data);
    out_result->executed = status == UMI_STATUS_OK;
    return finish_invocation(request, out_result, status);
}
