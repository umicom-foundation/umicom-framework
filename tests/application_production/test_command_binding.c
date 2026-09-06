/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_command_binding.c
 *
 * PURPOSE:
 *   Implement the test command binding behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | command_binding | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "test_fixture.h"

static size_t executed_count;

/* Approve a command only when the Framework resolved a real descriptor. */
static UmiStatus approve_command(
    const UmiApplicationCommandDescriptor *command,
    void *user_data)
{
    (void)user_data;
    return command != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* Record the action while leaving product behaviour outside the Framework binding. */
static UmiStatus execute_command(
    const UmiApplicationCommandDescriptor *command,
    void *user_data)
{
    (void)user_data;
    if (command == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    executed_count += 1U;
    return UMI_STATUS_OK;
}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    UmiApplicationProductionCommandBindings commands;
    UmiApplicationProductionCommandInvocationRequest request = {0};
    UmiApplicationProductionCommandInvocationResult result;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_production_command_bindings_build(&binding, &commands) == UMI_STATUS_OK);
    assert(commands.count > binding.experience->panel_count);
    assert(umi_application_production_command_bindings_find(&commands, UMI_APPLICATION_COMMAND_SELECT_LAYOUT, binding.experience->default_layout_id) != NULL);
    /* A selected layout is resolved, approved and passed to the product executor. */
    request.structure_size = sizeof(request);
    request.kind = UMI_APPLICATION_COMMAND_SELECT_LAYOUT;
    request.target_id = binding.experience->default_layout_id;
    request.require_approval = true;
    request.approval = approve_command;
    request.execute = execute_command;
    assert(umi_application_production_command_bindings_invoke(
               &commands, &request, &result) == UMI_STATUS_OK);
    assert(result.available && result.approval_requested && result.approved &&
           result.executed && executed_count == 1U);
    /* The caller may opt into an independent bounded history for palette activity. */
    {
        UmiApplicationProductionCommandJournal journal;
        umi_application_production_command_journal_init(&journal);
        request.journal = &journal;
        request.approval = approve_command;
        assert(umi_application_production_command_bindings_invoke(
                   &commands, &request, &result) == UMI_STATUS_OK);
        assert(journal.count == 1U);
        assert(umi_application_production_command_journal_at(&journal, 0U) != NULL);
        assert(umi_application_production_command_journal_at(&journal, 1U) == NULL);
        assert(strcmp(umi_application_production_command_journal_at(
                         &journal, 0U)->target_id,
                     binding.experience->default_layout_id) == 0);
        {
            UmiApplicationProductionCommandJournal checkpoint;
            UmiApplicationProductionCommandJournal restored;
            assert(umi_application_production_command_journal_capture(
                       &journal, &checkpoint) == UMI_STATUS_OK);
            memset(&restored, 0, sizeof(restored));
            assert(umi_application_production_command_journal_restore(
                       &checkpoint, &restored) == UMI_STATUS_OK);
            assert(restored.count == 1U && restored.next_sequence == 2U);
            checkpoint.entries[0].sequence = 0U;
            assert(umi_application_production_command_journal_validate(
                       &checkpoint) == UMI_STATUS_INVALID_STATE);
            assert(umi_application_production_command_journal_restore(
                       &checkpoint, &restored) == UMI_STATUS_INVALID_STATE);
            assert(restored.count == 1U && restored.entries[0].sequence == 1U);
        }
    }
    /* The temporary journal has gone out of scope, so later requests must not retain its address. */
    request.journal = NULL;
    /* A request that requires approval cannot silently execute without an approver. */
    request.approval = NULL;
    assert(umi_application_production_command_bindings_invoke(
               &commands, &request, &result) == UMI_STATUS_PERMISSION_DENIED);
    /* A capability provider can stop an otherwise valid command before execution. */
    {
        size_t index;
        const UmiApplicationProductionCommandBinding *restricted = NULL;
        for (index = 0U; index < commands.count; ++index) {
            if (commands.entries[index].command != NULL &&
                commands.entries[index].command->required_capability != NULL &&
                commands.entries[index].command->required_capability[0] != '\0') {
                restricted = &commands.entries[index];
                break;
            }
        }
        if (restricted != NULL) {
            request.kind = restricted->command->kind;
            request.target_id = restricted->command->target_id;
            request.require_approval = false;
            request.approval = NULL;
            request.capability_probe = umi_test_application_production_none_available;
            assert(umi_application_production_command_bindings_invoke(
                       &commands, &request, &result) == UMI_STATUS_UNAVAILABLE);
            assert(!result.available && !result.executed);
        }
    }
    /* A caller may resolve a command without executing it, which is useful for previews. */
    request.kind = UMI_APPLICATION_COMMAND_SELECT_LAYOUT;
    request.target_id = binding.experience->default_layout_id;
    request.capability_probe = NULL;
    request.execute = NULL;
    assert(umi_application_production_command_bindings_invoke(
               &commands, &request, &result) == UMI_STATUS_NOT_IMPLEMENTED);
    assert(result.available && result.approved && !result.executed);
    /* Invalid structure sizes are rejected before any command lookup occurs. */
    request.structure_size = 0U;
    assert(umi_application_production_command_bindings_invoke(
               &commands, &request, &result) == UMI_STATUS_INVALID_ARGUMENT);
    /* Unknown command kinds are rejected instead of being treated as stale IDs. */
    request.structure_size = sizeof(request);
    request.kind = (UmiApplicationCommandKind)99;
    assert(umi_application_production_command_bindings_invoke(
               &commands, &request, &result) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
