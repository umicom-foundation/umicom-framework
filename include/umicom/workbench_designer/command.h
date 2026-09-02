/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/command.h
 *
 * PURPOSE:
 *   Represent user intentions as auditable designer commands and execute them
 *   through semantic layout operations or specialised structural editors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_COMMAND_H
#define UMICOM_WORKBENCH_DESIGNER_COMMAND_H

#include "umicom/workbench_designer/properties.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer command data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerCommand {
    uint32_t structure_size;
    UmiWorkbenchDesignerCommandKind kind;
    char command_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char actor_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char correlation_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char target_node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char parent_node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char component_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char text_value[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    UmiWorkbenchLayoutNodeKind node_kind;
    UmiWorkbenchDesignerDropZone drop_zone;
    UmiWorkbenchDesignerRect rect_value;
    double number_value;
    size_t index_value;
    bool bool_value;
    uint64_t expected_revision;
    uint64_t timestamp_ms;
} UmiWorkbenchDesignerCommand;

/**
 * Represent the workbench designer command result data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerCommandResult {
    UmiStatus status;
    bool document_changed;
    bool selection_changed;
    uint64_t previous_revision;
    uint64_t resulting_revision;
    char affected_node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char message[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
} UmiWorkbenchDesignerCommandResult;

/**
 * Initialise workbench designer command from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_command_init(UmiWorkbenchDesignerCommand *command, UmiWorkbenchDesignerCommandKind kind, const char *command_id);
/**
 * Check that workbench designer command satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_designer_command_validate(const UmiWorkbenchDesignerCommand *command);
/**
 * Perform workbench designer command through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_workbench_designer_command_execute(UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerCommand *command, UmiWorkbenchDesignerCommandResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
