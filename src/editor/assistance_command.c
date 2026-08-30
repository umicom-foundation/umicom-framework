/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/assistance_command.c
 *
 * PURPOSE:
 *   Implement the stable Framework command catalogue consumed by Studio and
 *   future lightweight Umicom application shells.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/assistance_command.h"

#include <string.h>

#define COMMAND(command_kind, command_id, command_label, key, document, symbol, presented, view) \
    {                                                                                         \
        (uint32_t)sizeof(UmiEditorAssistanceCommandDescriptor),                              \
        UMI_EDITOR_ASSISTANCE_COMMAND_API_VERSION, command_kind, command_id,                  \
        command_label, key, document, symbol, presented, view                                \
    }

static const UmiEditorAssistanceCommandDescriptor COMMANDS[] = {
    COMMAND(UMI_EDITOR_ASSISTANCE_COMMAND_SHOW_HOVER,
            "editor.assistance.hover.show", "Show Hover", "Ctrl+K, Ctrl+I",
            1, 0, 0, 0),
    COMMAND(UMI_EDITOR_ASSISTANCE_COMMAND_PIN_HOVER,
            "editor.assistance.hover.pin", "Pin Hover", "", 1, 0, 1, 0),
    COMMAND(UMI_EDITOR_ASSISTANCE_COMMAND_DISMISS_HOVER,
            "editor.assistance.hover.dismiss", "Dismiss Hover", "Escape",
            0, 0, 1, 0),
    COMMAND(UMI_EDITOR_ASSISTANCE_COMMAND_TRIGGER_SIGNATURE_HELP,
            "editor.assistance.signature.trigger", "Trigger Signature Help",
            "Ctrl+Shift+Space", 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_ASSISTANCE_COMMAND_NEXT_SIGNATURE,
            "editor.assistance.signature.next", "Next Signature", "Alt+Down",
            1, 0, 1, 0),
    COMMAND(UMI_EDITOR_ASSISTANCE_COMMAND_PREVIOUS_SIGNATURE,
            "editor.assistance.signature.previous", "Previous Signature",
            "Alt+Up", 1, 0, 1, 0),
    COMMAND(UMI_EDITOR_ASSISTANCE_COMMAND_NEXT_PARAMETER,
            "editor.assistance.parameter.next", "Next Parameter", "",
            1, 0, 1, 0),
    COMMAND(UMI_EDITOR_ASSISTANCE_COMMAND_PREVIOUS_PARAMETER,
            "editor.assistance.parameter.previous", "Previous Parameter", "",
            1, 0, 1, 0),
    COMMAND(UMI_EDITOR_ASSISTANCE_COMMAND_DISMISS_SIGNATURE_HELP,
            "editor.assistance.signature.dismiss", "Dismiss Signature Help",
            "Escape", 0, 0, 1, 0),
    COMMAND(UMI_EDITOR_ASSISTANCE_COMMAND_SHOW_QUICK_DOCUMENTATION,
            "editor.assistance.documentation.show", "Show Quick Documentation",
            "Ctrl+Q", 1, 1, 0, 1),
    COMMAND(UMI_EDITOR_ASSISTANCE_COMMAND_PIN_QUICK_DOCUMENTATION,
            "editor.assistance.documentation.pin", "Pin Quick Documentation",
            "", 1, 1, 1, 1),
    COMMAND(UMI_EDITOR_ASSISTANCE_COMMAND_FOCUS_QUICK_DOCUMENTATION,
            "editor.assistance.documentation.focus", "Focus Quick Documentation",
            "", 0, 0, 1, 1),
    COMMAND(UMI_EDITOR_ASSISTANCE_COMMAND_OPEN_DOCUMENTATION_SOURCE,
            "editor.assistance.documentation.open-source",
            "Open Documentation Source", "", 0, 0, 1, 0),
    COMMAND(UMI_EDITOR_ASSISTANCE_COMMAND_REFRESH,
            "editor.assistance.refresh", "Refresh Editor Assistance", "",
            1, 0, 0, 0)
};

#undef COMMAND

size_t umi_editor_assistance_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiEditorAssistanceCommandDescriptor *
umi_editor_assistance_command_at(size_t index)
{
    return index < umi_editor_assistance_command_count() ? &COMMANDS[index]
                                                         : NULL;
}

const UmiEditorAssistanceCommandDescriptor *
umi_editor_assistance_command_find(const char *id)
{
    size_t index;

    if (id == NULL) return NULL;
    for (index = 0U; index < umi_editor_assistance_command_count(); ++index) {
        if (strcmp(COMMANDS[index].id, id) == 0) return &COMMANDS[index];
    }
    return NULL;
}

const UmiEditorAssistanceCommandDescriptor *
umi_editor_assistance_command_for_kind(UmiEditorAssistanceCommandKind kind)
{
    size_t index;

    for (index = 0U; index < umi_editor_assistance_command_count(); ++index) {
        if (COMMANDS[index].kind == kind) return &COMMANDS[index];
    }
    return NULL;
}
