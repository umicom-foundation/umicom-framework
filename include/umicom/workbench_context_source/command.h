/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/command.h
 *
 * PURPOSE:
 *   Define the reusable source command contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_COMMAND_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_COMMAND_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source command data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextSourceCommand {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceCommand;

/**
 * Initialise workbench context source command from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_command_init(
    UmiWorkbenchContextSourceCommand *record,
    const char *record_id);
/**
 * Check that workbench context source command satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_command_validate(
    const UmiWorkbenchContextSourceCommand *record);
/**
 * Provide the workbench context source command set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_command_set_source(
    UmiWorkbenchContextSourceCommand *record,
    const char *source_id);
/**
 * Provide the workbench context source command set panel operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_command_set_panel(
    UmiWorkbenchContextSourceCommand *record,
    const char *panel_id);
/**
 * Provide the workbench context source command set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_command_set_subject(
    UmiWorkbenchContextSourceCommand *record,
    const char *subject_id);
/**
 * Provide the workbench context source command set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_command_set_group(
    UmiWorkbenchContextSourceCommand *record,
    const char *group_id);
/**
 * Provide the workbench context source command set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_command_set_label(
    UmiWorkbenchContextSourceCommand *record,
    const char *label);
/**
 * Provide the workbench context source command hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_source_command_hash(
    const UmiWorkbenchContextSourceCommand *record);
/**
 * Provide the workbench context source command touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_source_command_touch(
    UmiWorkbenchContextSourceCommand *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
