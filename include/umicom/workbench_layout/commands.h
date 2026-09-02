/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/commands.h
 *
 * PURPOSE:
 *   Define typed commands handled by the Layout Slave Controller and keep all mutations behind one authoritative lifecycle boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_COMMANDS_H
#define UMICOM_WORKBENCH_LAYOUT_COMMANDS_H

#include "umicom/workbench_layout/operation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench layout command data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchLayoutCommand {
    uint32_t structure_size;
    UmiWorkbenchLayoutCommandKind kind;
    char command_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char actor_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char correlation_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char causation_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char layout_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char source_layout_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char template_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char session_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char name[UMI_WORKBENCH_LAYOUT_NAME_CAPACITY];
    char text[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    UmiWorkbenchLayoutOperation operation;
    uint64_t expected_revision;
    uint64_t timestamp_ms;
    bool bool_value;
} UmiWorkbenchLayoutCommand;

/**
 * Represent the workbench layout command result data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutCommandResult {
    uint32_t structure_size;
    UmiStatus status;
    UmiWorkbenchLayoutCommandKind kind;
    char command_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char layout_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    uint64_t previous_revision;
    uint64_t resulting_revision;
    bool changed;
    char message[UMI_WORKBENCH_LAYOUT_ERROR_CAPACITY];
} UmiWorkbenchLayoutCommandResult;

/**
 * Initialise workbench layout command from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_command_init(
    UmiWorkbenchLayoutCommand *command,
    UmiWorkbenchLayoutCommandKind kind,
    const char *command_id,
    const char *actor_id,
    const char *correlation_id);

/**
 * Check that workbench layout command satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_workbench_layout_command_validate(
    const UmiWorkbenchLayoutCommand *command);

/**
 * Provide the workbench layout command mutates state operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_command_mutates_state(
    const UmiWorkbenchLayoutCommand *command);

/**
 * Provide the workbench layout command requires layout operation used by this module and
 * its client applications.
 */
bool umi_workbench_layout_command_requires_layout(
    const UmiWorkbenchLayoutCommand *command);

/**
 * Provide the workbench layout command requires unlocked layout operation used by this
 * module and its client applications.
 */
bool umi_workbench_layout_command_requires_unlocked_layout(
    const UmiWorkbenchLayoutCommand *command);

/**
 * Initialise workbench layout command result from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_layout_command_result_init(
    UmiWorkbenchLayoutCommandResult *result,
    const UmiWorkbenchLayoutCommand *command);

#ifdef __cplusplus
}
#endif

#endif
