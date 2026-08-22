/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/command.h
 *
 * PURPOSE:
 *   Define the reusable source command contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_COMMAND_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_COMMAND_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_context_source_command_init(
    UmiWorkbenchContextSourceCommand *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_command_validate(
    const UmiWorkbenchContextSourceCommand *record);
UmiStatus umi_workbench_context_source_command_set_source(
    UmiWorkbenchContextSourceCommand *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_command_set_panel(
    UmiWorkbenchContextSourceCommand *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_command_set_subject(
    UmiWorkbenchContextSourceCommand *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_command_set_group(
    UmiWorkbenchContextSourceCommand *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_command_set_label(
    UmiWorkbenchContextSourceCommand *record,
    const char *label);
uint64_t umi_workbench_context_source_command_hash(
    const UmiWorkbenchContextSourceCommand *record);
void umi_workbench_context_source_command_touch(
    UmiWorkbenchContextSourceCommand *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
