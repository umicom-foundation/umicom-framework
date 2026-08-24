/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/command_context.h
 *
 * PURPOSE:
 *   Carry current IDE selections into stable Framework command handlers without
 *   toolkit event types.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_COMMAND_CONTEXT_H
#define UMICOM_IDE_INTEGRATION_COMMAND_CONTEXT_H

#include "umicom/ide_integration/selection.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdeCommandContext {
    UmiIdeEditorSelection editor_selection;
    size_t problem_index;
    size_t diagnostic_index;
    char test_item_id[UMI_IDE_INTEGRATION_ID_CAPACITY];
    char source_control_path[UMI_IDE_INTEGRATION_PATH_CAPACITY];
    uint32_t source_control_line;
    char symbol_id[UMI_IDE_INTEGRATION_ID_CAPACITY];
    char ai_task_id[UMI_IDE_INTEGRATION_ID_CAPACITY];
    char ai_request_id[UMI_IDE_INTEGRATION_ID_CAPACITY];
    char ai_session_id[UMI_IDE_INTEGRATION_ID_CAPACITY];
    char argument[UMI_IDE_INTEGRATION_TEXT_CAPACITY];
    int has_editor_selection;
} UmiIdeCommandContext;

void umi_ide_command_context_init(UmiIdeCommandContext *context);

#ifdef __cplusplus
}
#endif
#endif
