/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workspace_search_command.h
 *
 * PURPOSE:
 *   Publish stable command identities and enablement requirements for complete
 *   workspace search, result navigation, replacement preview and atomic apply.
 *
 * DESIGN:
 *   Command semantics belong to Umicom Framework. Applications contribute only
 *   menu, toolbar, command-centre and view placement for these identities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKSPACE_SEARCH_COMMAND_H
#define UMICOM_EDITOR_WORKSPACE_SEARCH_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_API_VERSION 1U

typedef enum UmiEditorWorkspaceSearchCommandKind {
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_OPEN = 1,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_EXECUTE = 2,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_REFRESH_INDEX = 3,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_NEXT_RESULT = 4,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_PREVIOUS_RESULT = 5,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_TOGGLE_CASE = 6,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_TOGGLE_REGEX = 7,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_TOGGLE_WHOLE_WORD = 8,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_TOGGLE_MULTILINE = 9,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_TOGGLE_OVERLAP = 10,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_ADD_INCLUDE = 11,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_ADD_EXCLUDE = 12,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_CLEAR_RESULTS = 13,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_COPY_RESULTS = 14,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_EXPORT_RESULTS = 15,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_OPEN_REPLACE = 16,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_BUILD_PREVIEW = 17,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_SELECT_ALL = 18,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_CLEAR_SELECTION = 19,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_PREPARE_REPLACE = 20,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_APPLY_REPLACE = 21,
    UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_CANCEL_REPLACE = 22
} UmiEditorWorkspaceSearchCommandKind;

typedef struct UmiEditorWorkspaceSearchCommandDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorWorkspaceSearchCommandKind kind;
    const char *id;
    const char *label;
    const char *default_keybinding;
    int requires_index;
    int requires_results;
    int requires_replacement;
    int requires_confirmation;
    int mutates_workspace;
} UmiEditorWorkspaceSearchCommandDescriptor;

size_t umi_editor_workspace_search_command_count(void);
const UmiEditorWorkspaceSearchCommandDescriptor *
umi_editor_workspace_search_command_at(size_t position);
const UmiEditorWorkspaceSearchCommandDescriptor *
umi_editor_workspace_search_command_find(const char *command_id);
const UmiEditorWorkspaceSearchCommandDescriptor *
umi_editor_workspace_search_command_for_kind(
    UmiEditorWorkspaceSearchCommandKind kind);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_WORKSPACE_SEARCH_COMMAND_H */
