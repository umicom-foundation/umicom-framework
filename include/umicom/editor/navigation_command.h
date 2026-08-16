/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/navigation_command.h
 *
 * PURPOSE:
 *   Publish canonical source-navigation command identities, labels and default
 *   keybindings while allowing each application to choose menu placement.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_NAVIGATION_COMMAND_H
#define UMICOM_EDITOR_NAVIGATION_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/navigation_result.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_NAVIGATION_COMMAND_API_VERSION 1U
#define UMI_EDITOR_NAVIGATION_COMMAND_ID_CAPACITY 96U
#define UMI_EDITOR_NAVIGATION_COMMAND_LABEL_CAPACITY 128U
#define UMI_EDITOR_NAVIGATION_COMMAND_KEY_CAPACITY 48U

typedef enum UmiEditorNavigationCommandKind {
    UMI_EDITOR_NAVIGATION_COMMAND_GO_TO_DEFINITION = 1,
    UMI_EDITOR_NAVIGATION_COMMAND_PEEK_DEFINITION = 2,
    UMI_EDITOR_NAVIGATION_COMMAND_GO_TO_DECLARATION = 3,
    UMI_EDITOR_NAVIGATION_COMMAND_GO_TO_TYPE_DEFINITION = 4,
    UMI_EDITOR_NAVIGATION_COMMAND_GO_TO_IMPLEMENTATION = 5,
    UMI_EDITOR_NAVIGATION_COMMAND_FIND_REFERENCES = 6,
    UMI_EDITOR_NAVIGATION_COMMAND_GO_BACK = 7,
    UMI_EDITOR_NAVIGATION_COMMAND_GO_FORWARD = 8,
    UMI_EDITOR_NAVIGATION_COMMAND_DOCUMENT_SYMBOLS = 9,
    UMI_EDITOR_NAVIGATION_COMMAND_WORKSPACE_SYMBOLS = 10,
    UMI_EDITOR_NAVIGATION_COMMAND_FOCUS_BREADCRUMBS = 11,
    UMI_EDITOR_NAVIGATION_COMMAND_TOGGLE_OUTLINE = 12
} UmiEditorNavigationCommandKind;

typedef struct UmiEditorNavigationCommandDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorNavigationCommandKind kind;
    char id[UMI_EDITOR_NAVIGATION_COMMAND_ID_CAPACITY];
    char label[UMI_EDITOR_NAVIGATION_COMMAND_LABEL_CAPACITY];
    char default_key[UMI_EDITOR_NAVIGATION_COMMAND_KEY_CAPACITY];
    UmiEditorNavigationQueryKind query_kind;
    int requires_symbol;
    int opens_multiple_results;
} UmiEditorNavigationCommandDescriptor;

size_t umi_editor_navigation_command_count(void);
const UmiEditorNavigationCommandDescriptor *
umi_editor_navigation_command_at(size_t index);
const UmiEditorNavigationCommandDescriptor *
umi_editor_navigation_command_find(const char *id);
const UmiEditorNavigationCommandDescriptor *
umi_editor_navigation_command_for_kind(UmiEditorNavigationCommandKind kind);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_NAVIGATION_COMMAND_H */
