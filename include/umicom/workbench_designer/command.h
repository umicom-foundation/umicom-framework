/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/command.h
 *
 * PURPOSE:
 *   Represent user intentions as auditable designer commands and execute them
 *   through semantic layout operations or specialised structural editors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_COMMAND_H
#define UMICOM_WORKBENCH_DESIGNER_COMMAND_H

#include "umicom/workbench_designer/properties.h"

#ifdef __cplusplus
extern "C" {
#endif


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

typedef struct UmiWorkbenchDesignerCommandResult {
    UmiStatus status;
    bool document_changed;
    bool selection_changed;
    uint64_t previous_revision;
    uint64_t resulting_revision;
    char affected_node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char message[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
} UmiWorkbenchDesignerCommandResult;

void umi_workbench_designer_command_init(UmiWorkbenchDesignerCommand *command, UmiWorkbenchDesignerCommandKind kind, const char *command_id);
UmiStatus umi_workbench_designer_command_validate(const UmiWorkbenchDesignerCommand *command);
UmiStatus umi_workbench_designer_command_execute(UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerCommand *command, UmiWorkbenchDesignerCommandResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
