/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/assistance_command.h
 *
 * PURPOSE:
 *   Publish canonical command identities and enablement requirements for the
 *   Framework-owned hover, signature and quick-documentation services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_ASSISTANCE_COMMAND_H
#define UMICOM_EDITOR_ASSISTANCE_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_ASSISTANCE_COMMAND_API_VERSION 1U
#define UMI_EDITOR_ASSISTANCE_COMMAND_ID_CAPACITY 96U
#define UMI_EDITOR_ASSISTANCE_COMMAND_LABEL_CAPACITY 128U
#define UMI_EDITOR_ASSISTANCE_COMMAND_KEY_CAPACITY 48U

typedef enum UmiEditorAssistanceCommandKind {
    UMI_EDITOR_ASSISTANCE_COMMAND_SHOW_HOVER = 1,
    UMI_EDITOR_ASSISTANCE_COMMAND_PIN_HOVER = 2,
    UMI_EDITOR_ASSISTANCE_COMMAND_DISMISS_HOVER = 3,
    UMI_EDITOR_ASSISTANCE_COMMAND_TRIGGER_SIGNATURE_HELP = 4,
    UMI_EDITOR_ASSISTANCE_COMMAND_NEXT_SIGNATURE = 5,
    UMI_EDITOR_ASSISTANCE_COMMAND_PREVIOUS_SIGNATURE = 6,
    UMI_EDITOR_ASSISTANCE_COMMAND_NEXT_PARAMETER = 7,
    UMI_EDITOR_ASSISTANCE_COMMAND_PREVIOUS_PARAMETER = 8,
    UMI_EDITOR_ASSISTANCE_COMMAND_DISMISS_SIGNATURE_HELP = 9,
    UMI_EDITOR_ASSISTANCE_COMMAND_SHOW_QUICK_DOCUMENTATION = 10,
    UMI_EDITOR_ASSISTANCE_COMMAND_PIN_QUICK_DOCUMENTATION = 11,
    UMI_EDITOR_ASSISTANCE_COMMAND_FOCUS_QUICK_DOCUMENTATION = 12,
    UMI_EDITOR_ASSISTANCE_COMMAND_OPEN_DOCUMENTATION_SOURCE = 13,
    UMI_EDITOR_ASSISTANCE_COMMAND_REFRESH = 14
} UmiEditorAssistanceCommandKind;

typedef struct UmiEditorAssistanceCommandDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorAssistanceCommandKind kind;
    char id[UMI_EDITOR_ASSISTANCE_COMMAND_ID_CAPACITY];
    char label[UMI_EDITOR_ASSISTANCE_COMMAND_LABEL_CAPACITY];
    char default_key[UMI_EDITOR_ASSISTANCE_COMMAND_KEY_CAPACITY];
    int requires_document;
    int requires_symbol;
    int requires_presented_assistance;
    int opens_view;
} UmiEditorAssistanceCommandDescriptor;

size_t umi_editor_assistance_command_count(void);
const UmiEditorAssistanceCommandDescriptor *
umi_editor_assistance_command_at(size_t index);
const UmiEditorAssistanceCommandDescriptor *
umi_editor_assistance_command_find(const char *id);
const UmiEditorAssistanceCommandDescriptor *
umi_editor_assistance_command_for_kind(UmiEditorAssistanceCommandKind kind);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_ASSISTANCE_COMMAND_H */
